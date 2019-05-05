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

    //移除监听事件
    void Remove();

    //处理活动事件
    void HandleEvent(uint64_t rcv_time);

    bool IsWriting() const { return events_ & kWriteEvent; }
    bool IsNoneEvent() const { return events_ == kNoneEvent; }

    const char* name() const { return name_; }
    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_revents(int revents) { revents_ = revents; }

    int stauts() const { return status_; }
    void set_status(int status) { status_ = status; }

    const EventLoop* OwnerLoop() const { return event_loop_; }

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

    //在poll中的监听状态
    //1.kNew在epoll中添加监听
    //2.kAdded表明已经在epoll中监听，
    //3.kDel表明禁止epoll中监听
    int status_;

    bool handling_; //是否处于事件处理中（当channel析构时，应当为false）

    //对方断开TCP连接，事件触发HandleEvent，回调用户提供的close_cb，
    //在用户的回调中有可能会释放了TCPConnectionPtr的持有，导致引用计
    //数有可能为0引发析构，这时候本身包含的Channel也会被析构，等于说
    //HandelEvent执行一半的时候对象被析构了，程序立刻core dump了
    //这种情况下需要增加保护
    bool tied_;                 //绑定该Chanel
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
