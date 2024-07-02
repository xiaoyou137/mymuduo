#ifndef EPOLLPOLLER_H
#define EPOLLPOLLER_H

#include "Poller.h"

#include <vector>
#include <sys/epoll.h>

// 前置声明
class EventLoop;

/**
 * @brief Epoll io 多路复用的封装
 */
class EpollPoller : public Poller
{
public:
    EpollPoller(EventLoop *loop);
    ~EpollPoller() override;

    // 重写Poller的虚函数
    void updateChannel(Channel *channel) override;
    void removeChannel(Channel *channel) override;
    Timestamp poll(int timeoutMs, ChannelList *activeChannels) override;

private:
    using EventList = std::vector<epoll_event>;
    static const int kInitEventListSize = 16;

    // 填写活跃的连接
    void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
    // 更新channel
    void update(int operation, Channel *channel);

    int epollFd_;
    EventList events_; // 监听的事件列表
};

#endif
