#include "EpollPoller.h"
#include "Logger.h"

#include <errno.h>
#include <unistd.h>
#include <sys/epoll.h>

const int kNew = -1;    // channel未添加到poller中
const int kAdded = 1;   // channel已添加到poller中
const int kDeleted = 2; // channel已从poller中删除

EpollPoller::EpollPoller(EventLoop *loop)
    : Poller(loop), epollFd_(epoll_create1(EPOLL_CLOEXEC)), events_(kInitEventListSize)
{
    if (epollFd_ < 0)
    {
        LOG_FATAL("epoll create1 failed : %d !", errno);
    }
}

EpollPoller::~EpollPoller()
{
    ::close(epollFd_);
}

void EpollPoller::updateChannel(Channel *channel)
{
    int index = channel->index();

    if (index == kNew || index == kDeleted)
    {
        if (index == kNew)
        {
            int fd = channel->fd();
            channelMap_[fd] = channel;
        }

        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else // 已经在channelMap_中了
    {
        if (channel->isNoneEvent())
        {
            channel->set_index(kDeleted);
            update(EPOLL_CTL_DEL, channel);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EpollPoller::removeChannel(Channel *channel)
{
    int fd = channel->fd();
    channelMap_.erase(fd);

    int index = channel->index();
    if (index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}

Timestamp EpollPoller::poll(int timeoutMs, ChannelList *activeChannels)
{
    LOG_DEBUG("fd total count:%d\n", channelMap_.size());

    int numEvents = ::epoll_wait(epollFd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);
    int saveerr = errno;

    Timestamp now(Timestamp::now());

    if (numEvents > 0) // 有事件发生
    {
        LOG_DEBUG("%d events happened!\n", numEvents);
        fillActiveChannels(numEvents, activeChannels);
        // 如有需要，扩容
        if (numEvents == events_.size())
        {
            events_.resize(events_.size() * 2);
        }
    }
    else if (numEvents == 0)
    {
        // 没有发生任何事件
        LOG_DEBUG("TIMEOUT!\n");
    }
    else
    {
        // 发生错误
        if (saveerr != EINTR)
        {
            errno = saveerr;
            LOG_ERROR("EpollPoller::poll err!\n");
        }
    }
    return now;
}

void EpollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const
{
    for (int i = 0; i < numEvents; i++)
    {
        Channel *channel = static_cast<Channel *>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        activeChannels->emplace_back(channel); // eventloop就拿到了它的poller返回的所有发生事件的channel列表了。
    }
}

void EpollPoller::update(int operation, Channel *channel)
{
    epoll_event event;
    int fd = channel->fd();
    event.data.fd = fd;
    event.data.ptr = channel;
    event.events = channel->events();

    if (::epoll_ctl(epollFd_, operation, fd, &event) < 0)
    {
        if (operation == EPOLL_CTL_DEL)
        {
            LOG_ERROR("epoll ctl del failed: %d", errno);
        }
        else
        {
            LOG_FATAL("epoll ctl add/mod failed: %d", errno);
        }
    }
}
