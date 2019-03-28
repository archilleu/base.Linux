//---------------------------------------------------------------------------
#include <sys/epoll.h>
#include <sys/poll.h>
#include <cassert>
#include <sstream>
#include "channel.h"
#include "event_loop.h"
#include "net_logger.h"
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;
//---------------------------------------------------------------------------
Channel::Channel(EventLoop* event_loop, int fd, const char* name)
:   event_loop_(event_loop),
    name_(name),
    fd_(fd),
    events_(kNoneEvent),
    revents_(kNoneEvent),
    status_(1),  //kNew,在epoller里面定义
    handling_(false),
    tied_(false)
{
    NetLogger_trace("Channel ctor this(%p), fd:%d", this, fd_);
    assert(0 != event_loop_);
    assert(0 < fd_);

    return;
}
//---------------------------------------------------------------------------
Channel::~Channel()
{
    NetLogger_trace("channel dtor this(%p), fd:%d", this, fd_);
    assert(false == handling_);

    return;
}
//---------------------------------------------------------------------------
void Channel::Tie(const std::shared_ptr<void>& owner)
{
    tie_ = owner;
    tied_ = true;

    return;
}
//---------------------------------------------------------------------------
void Channel::Remove()
{
    event_loop_->RemoveChannel(this);
    return;
}
//---------------------------------------------------------------------------
void Channel::HandleEvent(uint64_t rcv_time)
{
    if(tied_)
    {
        std::shared_ptr<void> guard = tie_.lock();
        if(guard)
        {
            HandleEvent_(rcv_time);
        }
    }
    else
    {
        HandleEvent_(rcv_time);
    }

    return;
}
//---------------------------------------------------------------------------
void Channel::HandleEvent_(uint64_t rcv_time)
{
    NetLogger_trace("Channel(%s) handling event: fd:%d, events:%s", name_, fd_, REventsToString_().c_str());

    //标记正在处理事件
    handling_ = true;

    //优先处理断线情况
    if((EPOLLHUP | EPOLLRDHUP) & revents_)
    {
        if(close_cb_)
        {
            close_cb_();
        }
    
        handling_ = false;
        return;
    }

    //出错
    if(revents_ & EPOLLERR)
    {
        if(error_cb_)
        {
            error_cb_();
        }

        handling_ = false;
        return;
    }

    //可读
    if(revents_ & (EPOLLIN|EPOLLPRI))
    {
        if(read_cb_)
        {
            read_cb_(rcv_time);
        }

        //处理可写
        //return;
    }

    //可写
    if(revents_ & EPOLLOUT)
    {
        if(write_cb_)
        {
            write_cb_();
        }

        handling_ = false;
    }

    handling_ = false;
    return;
}
//---------------------------------------------------------------------------
std::string Channel::REventsToString_()
{
    return EventsToString_(revents_);
}
//---------------------------------------------------------------------------
std::string Channel::EventsToString_()
{
    return EventsToString_(events_);
}
//---------------------------------------------------------------------------
void Channel::UpdateEvent()
{
    event_loop_->UpdateChannel(this);
}
//---------------------------------------------------------------------------
std::string Channel::EventsToString_(int ev)
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
//---------------------------------------------------------------------------
}//namespace net
//---------------------------------------------------------------------------
