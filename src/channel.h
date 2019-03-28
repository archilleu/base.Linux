//---------------------------------------------------------------------------
#ifndef NET_CHANNEL_H_
#define NET_CHANNEL_H_
//---------------------------------------------------------------------------
#include <memory>
#include <functional>
#include "../thirdpart/base/include/noncopyable.h"
//---------------------------------------------------------------------------
namespace net
{

class EventLoop;

class Channel : public base::Noncopyable
{
public:
    using EventCallback = std::function<void (void)>;
    using ReadEventCallback = std::function<void (uint64_t)>;

public:
    Channel(EventLoop* event_loop, int fd, const char* name="");
    ~Channel();

public:
    //为防止拥有该Channel的对象析构导致自己在处理事件过程中提前析构,需要增加对Channel的保护
    void Tie(const std::shared_ptr<void>& owner);

    //设置回调
    void set_read_cb(const ReadEventCallback& cb)   { read_cb_ = cb; };
    void set_write_cb(const EventCallback& cb)      { write_cb_ = cb; };
    void set_close_cb(const EventCallback& cb)      { close_cb_ = cb; };
    void set_error_cb(const EventCallback& cb)      { error_cb_ = cb; };

    //更改事件
    void EnableReading()    { events_ |= kReadEvent; UpdateEvent(); }
    void DisableReading()   { events_ &= ~kReadEvent; UpdateEvent(); }
    void EnableWriting()    { events_ |= kWriteEvent; UpdateEvent(); }
    void DisableWriting()   { events_ &= ~kWriteEvent; UpdateEvent(); }
    void DisableAll()       { events_ = kNoneEvent; UpdateEvent(); }
    void Remove();  //通知EventLoop移除channle

    //处理活动事件
    void HandleEvent(uint64_t rcv_time);

    //是否关注写事件,用于buffer的写(如果写缓存还有,则该事件一直会被关注
    bool IsWriting() const      { return events_ & kWriteEvent; }
    bool IsNoneEvent() const    { return events_ == kNoneEvent; }

    const char* name() const    { return name_; }
    int fd() const  { return fd_; }
    int events() const  { return events_; }
    void set_revents(int revents)   { revents_ = revents; }

    int stauts() const  { return status_; }
    void set_status(int status) { status_ = status; }

    const EventLoop* OwnerLoop() const  { return event_loop_; }

    //调试接口
    std::string REventsToString_();
    std::string EventsToString_();

private:
    void HandleEvent_(uint64_t rcv_time);

    void UpdateEvent(); //向poller更新事件监听状态

    std::string EventsToString_(int ev);

private:
    EventLoop* event_loop_;
    const char* name_;
    const int fd_;
    int events_;
    int revents_;

    //在poll中的监听状态，kNew 在epoll中添加监听，kAdded表明已经在epoll中监听，
    //而kDel表明禁止epoll中监听
    int status_;

    bool handling_; //是否处于事件处理中（当channel析构时，应当为false）

    //为防止拥有该Channel的对象析构导致自己在处理事件过程中TCPConnection析构导致
    //Channel析构,需要增加对Channel的保护
    bool tied_;                 //是否绑定了
    std::weak_ptr<void> tie_;   //channel拥有者

    ReadEventCallback read_cb_;
    EventCallback write_cb_;
    EventCallback close_cb_;
    EventCallback error_cb_;

private:
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //NET_CHANNEL_H_
