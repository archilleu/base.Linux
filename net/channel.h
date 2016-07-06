//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_CHANNEL_H_
#define BASE_LINUX_NET_CHANNEL_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
#include "../base/timestamp.h"
//---------------------------------------------------------------------------
namespace net
{

class EventLoop;

class Channel
{
public:
    typedef std::function<void (void)>              EventCallback;
    typedef std::function<void (base::Timestamp)>   EventCallbackRead;
    
public:
    Channel(EventLoop* loop, int fd);
    ~Channel();
    
    //为防止拥有该Channel的对象析构导致自己在处理事件过程中提前析构,需要增加对Channel的保护
    void Tie(const std::shared_ptr<void>& owner);

    //设置回调函数
    void set_callback_write (EventCallback&& callback)        { callback_write_ = std::move(callback); }
    void set_callback_close (EventCallback&& callback)        { callback_close_ = std::move(callback); }
    void set_callback_error (EventCallback&& callback)        { callback_error_ = std::move(callback); }
    void set_callback_read  (EventCallbackRead&& callback)    { callback_read_ = std::move(callback); }

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
    bool IsWriting()    { return events_ & kEventWrite; }
    bool IsNoneEvent()  { return events_ == kNone; }

    int     events()                    { return events_; }
    void    set_revents(int revents)    { revents_ = revents; }
    
    EventLoop* owner_loop() { return owner_loop_; }

    int     fd() const          { return fd_; }
    int     index() const       { return index_; }
    void    set_index(int idx)  { index_= idx; }

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
    int         index_;     //对应于在poller中channels_的idx
    bool        handling_;  //是否正在处理事件中,如果是,则该Channel不能删除

    //为防止拥有该Channel的对象析构导致自己在处理事件过程中提前析构,需要增加对Channel的保护
    bool                tied_;
    std::weak_ptr<void> tie_;//Channel的拥有者


    EventCallback       callback_write_;    //写事件回调
    EventCallback       callback_close_;    //关闭事件回调
    EventCallback       callback_error_;    //错误事件回调
    EventCallbackRead   callback_read_;     //读事件回调

    static const int kNone;         //无事件
    static const int kEventRead;    //读事件
    static const int kEventWrite;   //写事件

protected:
    DISALLOW_COPY_AND_ASSIGN(Channel);
};

}//namespace net
//---------------------------------------------------------------------------
#endif //BASE_LINUX_NET_CHANNEL_H_

