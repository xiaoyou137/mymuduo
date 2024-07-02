#ifndef POLLER_H
#define POLLER_H

#include "noncopyable.h"
#include "Channel.h"

#include <unordered_map>
#include <vector>

// 前置声明
class EventLoop;

/**
* @brief muduo库中多路复用分发器的核心IO复用模块
*/
class Poller : noncopyable
{
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop* loop);
    virtual ~Poller() = default;

    // 给所有IO复用保留统一的接口
    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) = 0;
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;

    // 判断channel是否在当前poller中
    bool hasChannel(Channel* channel) const;

    // Eventloop可以通过该接口获取具体的Poller实例
    static Poller* newDefaultPoller(EventLoop* loop);
protected:
    using ChannelMap = std::unordered_map<int, Channel*>;
    ChannelMap channelMap_;
private:
    EventLoop* ownerLoop_;
}; 

#endif
