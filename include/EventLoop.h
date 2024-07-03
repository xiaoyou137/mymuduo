#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "noncopyable.h"
#include "Timestamp.h"
#include "CurrentThread.h"

#include <atomic>
#include <functional>
#include <mutex>
#include <vector>
#include <memory>


// 前置声明
class Poller;
class Channel;

/**
* @brief Eventloop时间循环类，主要包含了两大模块 Channel 和 Poller, 三者共同完成了 Reactor和多路事件分发器的角色
*/
class EventLoop : noncopyable
{
public:
    using Functor = std::function<void()>;
    EventLoop();
    ~EventLoop();

    // 开启事件循环
    void loop();
    // 退出事件循环
    void quit();

    Timestamp pollReturnTime() const { return pollReturnTime_; }

    // 在当前loop中执行cb
    void runInLoop(Functor cb);
    // 把cb放入队列，唤醒loop所在线程，执行cb
    void queueInLoop(Functor cb);

    // 唤醒loop所在线程
    void wakeup();

    // 通过loop调用poller的方法
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);

    // 判断当前线程是否loop所属线程
    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

private:
    // wakeup
    void handleRead();
    // 执行当前loop的回调
    void doPendingFunctors();

    using ChannelList = std::vector<Channel*>;
    
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
};

#endif
