//---------------------------------------------------------------------------
#include "channel.h"
#include "event_loop.h"
#include "net_log.h"

#include <sys/epoll.h>
//---------------------------------------------------------------------------
namespace net
{
const int Channel::kEventNormal = EPOLLRDHUP | EPOLLHUP | EPOLLERR;
const int Channel::kEventRead   = EPOLLIN | EPOLLPRI;
const int Channel::kEventWrite  = EPOLLOUT;
//---------------------------------------------------------------------------
Channel::Channel(EventLoop* loop, int vfd)
:   loop_(loop),
    fd_(vfd),
    events_(kEventNormal),
    revents_(0),
    status_(0),
    handling_(false)
{
    assert(0 != loop_);
    assert(0 < fd_);

    return;
}
//---------------------------------------------------------------------------
Channel::~Channel()
{
    assert(false == handling_);
    return;
}
//---------------------------------------------------------------------------
void Channel::HandleEvent(base::Timestamp rcv_time)
{
    //标记正在处理事件中
    handling_ = true;

    SystemLog_Debug("Handle event:fd:%d event:%s", fd_, REventsToString().c_str());

    //优先处理断线情况
    if((EPOLLHUP | EPOLLRDHUP) & events_)
    {
        if(callback_close_)
        {
//            handling_ = false;  //断线即是销毁对象的时候
            callback_close_();
        }
    }

    //出错情况
    if(EPOLLERR & revents_)
    {
        if(callback_error_)
        {
            callback_error_();
            handling_ = false;
        }

        return;
    }

    //可读
    if((EPOLLIN|EPOLLPRI|EPOLLHUP) & revents_)
    {
        if(callback_read_)
        {
            callback_read_(rcv_time);
        }
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
void Channel::Remove()
{
//    loop_->ChannelRemove(this);
}
//---------------------------------------------------------------------------
std::string Channel::REventsToString()
{
    return _EventsToString(fd_, events_);
}
//---------------------------------------------------------------------------
std::string Channel::EventsToString()
{
    return _EventsToString(fd_, revents_);
}
//---------------------------------------------------------------------------
void Channel::UpdateEvent()
{
}
//---------------------------------------------------------------------------
std::string Channel::_EventsToString(int vfd, int ev)
{
    std::ostringstream oss;
    oss << vfd << ": ";
    if(ev & EPOLLIN)    oss << "IN ";
    if(ev & EPOLLPRI)   oss << "PRI ";
    if(ev & EPOLLOUT)   oss << "OUT ";
    if(ev & EPOLLHUP)   oss << "HUP ";
    if(ev & EPOLLRDHUP) oss << "RDHUP ";
    if(ev & EPOLLERR)   oss << "ERR ";

    return oss.str();
}
}//namespace net
