//---------------------------------------------------------------------------
#include "channel.h"
#include "event_loop.h"
#include "net_log.h"

#include <sys/epoll.h>
//---------------------------------------------------------------------------
namespace net
{
const int Channel::kNone        = 0;  
const int Channel::kEventRead   = EPOLLIN | EPOLLPRI;
const int Channel::kEventWrite  = EPOLLOUT;
//---------------------------------------------------------------------------
Channel::Channel(EventLoop* loop, int _fd)
:   owner_loop_(loop),
    fd_(_fd),
    events_(kNone),//在TCPServer通知TCPConn对应成功建立连接前,loop都不能接收任何事件
    revents_(0),
    status_(1),
    handling_(false),
    tied_(false)
{
    SystemLog_Debug("Channel ctor");

    assert(0 != owner_loop_);
    assert(0 < fd_);

    return;
}
//---------------------------------------------------------------------------
Channel::~Channel()
{
    SystemLog_Debug("Channel dtor");

    assert(false == handling_);
    return;
}
//---------------------------------------------------------------------------
void Channel::Tie(const std::shared_ptr<void>& owner)
{
    tie_    = owner;
    tied_   = true;

    return;
}
//---------------------------------------------------------------------------
void Channel::HandleEvent(base::Timestamp rcv_time)
{
    if(true == tied_)
    {
        std::shared_ptr<void> guard = tie_.lock();
        if(guard)
        {
            _HandleEvent(rcv_time);
        }
    }
    else
    {
        _HandleEvent(rcv_time);
    }

    return;
}
//---------------------------------------------------------------------------
void Channel::Remove()
{
    owner_loop_->ChannelRemove(this);
    return;
}
//---------------------------------------------------------------------------
std::string Channel::REventsToString()
{
    return _EventsToString(revents_);
}
//---------------------------------------------------------------------------
std::string Channel::EventsToString()
{
    return _EventsToString(events_);
}
//---------------------------------------------------------------------------
void Channel::_HandleEvent(base::Timestamp rcv_time)
{
    //标记正在处理事件中
    handling_ = true;

    SystemLog_Debug("Handle event:fd:%d event:%s", fd_, REventsToString().c_str());

    //优先处理断线情况
    if((EPOLLHUP | EPOLLRDHUP) & revents_)
    {
        if(callback_close_)
        {
            callback_close_();
        }

        handling_ = false;
        return;
    }

    //出错情况
    if(EPOLLERR & revents_)
    {
        if(callback_error_)
        {
            callback_error_();
        }

        handling_ = false;
        return;
    }

    //可读
    if((EPOLLIN|EPOLLPRI) & revents_)
    {
        if(callback_read_)
        {
            callback_read_(rcv_time);
        }

        //同时可能有写操作
        //return;
    }
    
    //可写
    if(EPOLLOUT & revents_)
    {
        if(callback_write_)
        {
            callback_write_();
        }
    }

    handling_ = false;
    return;
}
//---------------------------------------------------------------------------
void Channel::UpdateEvent()
{
    owner_loop_->ChannelUpdate(this);
    return;
}
//---------------------------------------------------------------------------
std::string Channel::_EventsToString(int ev)
{
    std::ostringstream oss;
    oss << "<";
    if(ev & EPOLLIN)    oss << "IN ";
    if(ev & EPOLLPRI)   oss << "PRI ";
    if(ev & EPOLLOUT)   oss << "OUT ";
    if(ev & EPOLLHUP)   oss << "HUP ";
    if(ev & EPOLLRDHUP) oss << "RDHUP ";
    if(ev & EPOLLERR)   oss << "ERR ";
    oss << ">";

    return oss.str();
}
}//namespace net
