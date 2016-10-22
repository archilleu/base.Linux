//---------------------------------------------------------------------------
#ifndef NET_CHANNEL_H_
#define NET_CHANNEL_H_
//---------------------------------------------------------------------------
#include <functional>
#include <memory>
#include "../depend/base/include/timestamp.h"
//---------------------------------------------------------------------------
namespace net
{

class EventLoop;

class Channel
{
public:
    using EventCallback     = std::function<void (void)>;
    using EventCallbackRead = std::function<void (base::Timestamp)>;
    
public:
    Channel(EventLoop* loop, int fd);
    Channel(const Channel&) =delete;
    Channel& operator=(const Channel&) =delete;
    ~Channel();
    
    //为防止拥有该Channel的对象析构导致自己在处理事件过程中提前析构,需要增加对Channel的保护
    void Tie(const std::shared_ptr<void>& owner);

    //设置回调函数
    void set_callback_write(EventCallback&& callback)       { callback_write_ = std::move(callback); }
    void set_callback_close(EventCallback&& callback)       { callback_close_ = std::move(callback); }
    void set_callback_error(EventCallback&& callback)       { callback_error_ = std::move(callback); }
    void set_callback_read(EventCallbackRead&& callback)    { callback_read_ = std::move(callback); }

    //更改事件
    void ReadEnable()   { events_ |= kEventRead; UpdateEvent(); }
    void ReadDisable()  { events_ &= ~kEventRead; UpdateEvent(); }
    void WriteEnable()  { events_ |= kEventWrite; UpdateEvent(); }
    void WriteDisable() { events_ &= ~kEventRead; UpdateEvent(); }

    //关闭所有事件,一般用于关闭连接
    void DisableAll()   { events_ = kNone; UpdateEvent(); }

    //处理活动事件
    void HandleEvent(base::Timestamp rcv_time);

    //通知EventLoop移除Channel
    void Remove();

    //是否关注写事件,用于buffer的写(如果写缓存还有,则该事件一直会被关注
    bool IsWriting() const      { return events_ & kEventWrite; }
    bool IsNoneEvent() const    { return events_ == kNone; }

    int     events() const              { return events_; }
    void    set_revents(int revents)    { revents_ = revents; }
    
    const EventLoop* owner_loop() const { return owner_loop_; }

    int     fd() const          { return fd_; }
    int     status() const       { return status_; }
    void    set_status(int s)  { status_ = s; }

    //调试接口
    std::string REventsToString();
    std::string EventsToString();

private:
    void _HandleEvent(base::Timestamp rcv_time);

    void UpdateEvent();

    std::string _EventsToString(int ev);

private:
    EventLoop*  owner_loop_;//事件循环对象
    int         fd_;        //Channel关联的描述符
    int         events_;    //关注的事件
    int         revents_;   //触发的事件
    int         status_;    //在poll中的监听状态，kNew 在epoll中添加监听，kAdded表明已经在epoll中监听，而kDel表明禁止epoll中监听

    bool        handling_;  //是否正在处理事件中(用于调试，正常来说当Channel析构时，handing_为false) 

    //为防止拥有该Channel的对象析构导致自己在处理事件过程中TCPConnection析构导致Channel析构,需要增加对Channel的保护
    bool tied_;
    std::weak_ptr<void> tie_;//Channel的拥有者

    EventCallback       callback_write_;    //写事件回调
    EventCallback       callback_close_;    //关闭事件回调
    EventCallback       callback_error_;    //错误事件回调
    EventCallbackRead   callback_read_;     //读事件回调

    static const int kNone;         //无事件
    static const int kEventRead;    //读事件
    static const int kEventWrite;   //写事件
};

}//namespace net
//---------------------------------------------------------------------------
#endif //NET_CHANNEL_H_

