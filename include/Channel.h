#ifndef CHANNEL_H
#define CHANNEL_H

#include "Timestamp.h"
#include "noncopyable.h"

#include <functional>
#include <memory>

// 前置声明
class EventLoop;

/**
 * @brief Channel封装了sockfd和它感兴趣的事件（如EPOLLIN、EPOLLOUT）
 *         ， 并且绑定了从poller返回的具体事件
 */
class Channel : noncopyable
{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop *loop, int fd);
    ~Channel();

    // fd得到通知以后，处理时间
    void handleEvent(Timestamp receiveTime);

    // 设置回调函数
    void setReadCallback(const ReadEventCallback& cb) { readCallback_ = cb; }
    void setWriteCallback(const EventCallback& cb) { writeCallback_ = cb; }
    void setErrorCallback(const EventCallback& cb) { errorCallback_ = cb; }
    void setCloseCallback(const EventCallback& cb) { closeCallback_ = cb; }

    // 防止当Channel被remove后，还在执行回调
    void tie(std::shared_ptr<void>&);

    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_revents(int revt) { revents_ = revt; }

    // 设置fd相应的事件状态
    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kReadEvent; update(); }
    void diableAll() { events_ = kNoneEvent; update(); }

    // 返回fd响应的事件状态
    bool isNoneEvent() const { return events_ == kNoneEvent; }
    bool isReadEvent() const { return events_ == kReadEvent; }
    bool isWriteEvent() const { return events_ == kWriteEvent; }

    int index() const { return index_; }
    void set_index(int idx) { index_ = idx; }

    EventLoop* ownerLoop() const { return loop_; }
    void remove();

private:

    void update();
    void handleEventWithGuard(Timestamp receiveTIme);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_; // 事件循环
    const int fd_;    // fd, poller监听的对象
    int events_;      // 注册fd感兴趣的事件
    int revents_;     // poller返回的具体事件
    int index_;

    std::weak_ptr<void> tie_;
    bool tied_;

    // 因为Channel里面能够知道fd最终发生的具体事件，所以它负责调用具体回调接口。
    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
    EventCallback closeCallback_;
};

#endif
