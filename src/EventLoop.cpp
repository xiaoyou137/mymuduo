#include "EventLoop.h"
#include "Logger.h"
#include "Poller.h"
#include "Channel.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <errno.h>
#include <memory>

// 防止一个线程创建多个loop
__thread EventLoop* t_loopInThisThread = nullptr;

// 定义默认的poller超时时间
const int kPoolTimeoutMs = 10000;

// 创建wakeupFd_
int createEventfd()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(evtfd < 0)
    {
        LOG_FATAL("eventfd error: %d\n", errno);
    }
    return evtfd;
}

/**
* @brief eventloop的构造函数
    std::atomic_bool looping_; // 是否开启了事件循环
    std::atomic_bool quit_; // 是否要退出循环

    const pid_t threadId_; // 记录当前loop所在的线程id

    Timestamp pollReturnTime_; // poller返回发生事件的Timestamp
    std::unique_ptr<Poller> poller_; // 当前loop的poller

    int wakeupFd_; // 用于唤醒阻塞在poller中的loop
    std::unique_ptr<Channel> wakeupChannel_; // 用于绑定wakeupFd_的Channel

    ChannelList activeChannels_;

    std::atomic_bool callingPendingFunctors_; // 是否正在执行回调
    std::vector<Functor> pendingFunctors_; // 用于记录loop需要执行的回调操作
    std::mutex mutex_; // 用来保证pendingFunctors_的线程安全
*/
EventLoop::EventLoop()
    : looping_(false)
    , quit_(false)
    , threadId_(CurrentThread::tid())
    , poller_(Poller::newDefaultPoller(this))
    , wakeupFd_(createEventfd())
    , wakeupChannel_(std::unique_ptr<Channel>(new Channel(this, wakeupFd_)))
{
    LOG_DEBUG("Eventloop created %p in thread %d\n", this, threadId_);
    if(t_loopInThisThread)
    {
        LOG_FATAL("Another eventloop %p exists in this thread %d\n", t_loopInThisThread, CurrentThread::tid());
    }
    else
    {
        t_loopInThisThread = this;
    }

    // 设置wakeup
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop()
{
    looping_ = true;
    quit_ = false;

    LOG_DEBUG("Eventloop %p start looping\n", this);

    while(!quit_)
    {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPoolTimeoutMs, &activeChannels_);

        // 每一个发生事件的channel处理各自事件
        for(auto channel : activeChannels_)
        {
            channel->handleEvent(pollReturnTime_);
        }

        // 执行当前loop上的回调
        doPendingFunctors();
    }

    LOG_DEBUG("Eventloop %p stop looping\n", this);
    looping_ = false;
}

void EventLoop::quit()
{
    quit_ = false;
    if(!isInLoopThread())
    {
        wakeup();
    }
}

void EventLoop::runInLoop(Functor cb)
{
    if(isInLoopThread())
    {
        cb();
    }
    else
    {
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(Functor cb)
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        pendingFunctors_.emplace_back(cb);
    }

    /**
    * 如果当前线程不是loop所在线程，或者当前loop正在执行回调，执行完后又会等待在poller中，需要wakeup
    */
    if(!isInLoopThread() || callingPendingFunctors_)
    {
        wakeup();
    }
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t ret = ::write(wakeupFd_, &one, sizeof one);
    if(ret != sizeof one)
    {
        LOG_ERROR("Eventloop %p write %d bytes instead of 8 \n", this, ret);
    }
}

void EventLoop::updateChannel(Channel *channel)
{
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel)
{
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel)
{
    return poller_->hasChannel(channel);
}

void EventLoop::handleRead()
{
    uint64_t one;
    ssize_t ret = ::read(wakeupFd_, &one, sizeof one);
    if(ret != sizeof one)
    {
        LOG_ERROR("Eventloop %p read %d bytes instead of 8 \n", this, ret);
    }
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    { // 通过swap ，减少持有lock的时间，提高效率
        std::lock_guard<std::mutex> lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for(auto functor : functors)
    {
        functor();
    }

    callingPendingFunctors_ = false;

}
