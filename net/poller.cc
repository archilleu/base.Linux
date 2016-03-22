//---------------------------------------------------------------------------
#include "poller.h"
#include "channel.h"
#include "net_log.h"
#include <sys/epoll.h>
//---------------------------------------------------------------------------
namespace net
{
//---------------------------------------------------------------------------
Poller::Poller()
:   fd_nums_(0)
{
    efd_ = ::epoll_create1(EPOLL_CLOEXEC);
    if(0 > efd_)
    {
        SystemLog_Error("create epoll fd failed");
        assert(0);
    }

    event_list_.resize(64); //初始最多监听24个fd
    return;
}
//---------------------------------------------------------------------------
Poller::~Poller()
{
    close(efd_);
    return;
}
//---------------------------------------------------------------------------
base::Timestamp Poller::Poll(int timeout, ChannelList* active_channel_list)
{
    int nums = ::epoll_wait(efd_, static_cast<struct epoll_event*>(event_list_.data()), static_cast<int>(event_list_.size()), timeout*1000);
    base::Timestamp rcv_time = base::Timestamp::Now();
    
    //有事件
    if(0 < nums)
    {
        SystemLog_Debug("event nums:%d", nums);

        FillActiveChannel(nums, active_channel_list);
        return rcv_time;
    }

    //没有事件
    if(0 == nums)
    {
        //SystemLog_Debug("no events");
        return rcv_time;
    }

    //出错
    if(EINTR != errno)
    {
        char buffer[128];
        SystemLog_Error("epoll_wait error:no:%d, msg:%s", errno, strerror_r(errno, buffer, sizeof(buffer)));
        assert(0);
    }

    return rcv_time;
}
//---------------------------------------------------------------------------
void Poller::ChannelAdd(Channel* channel)
{
    int status = channel->status();
    if(Channel::CHANNEL_Added == status)
        return;

    channel->set_status(Channel::CHANNEL_Added);
    Update(EPOLL_CTL_ADD, channel);

    return;
}
//---------------------------------------------------------------------------
void Poller::ChannelMod(Channel* channel)
{
    int status = channel->status();
    if(Channel::CHANNEL_None == status)
        return;

    Update(EPOLL_CTL_MOD, channel);
    return;
}
//---------------------------------------------------------------------------
void Poller::ChannelDel(Channel* channel)
{
    int status = channel->status();
    if(Channel::CHANNEL_None == status)
        return;

    channel->set_status(Channel::CHANNEL_None);
    Update(EPOLL_CTL_DEL, channel);

    return;
}
//---------------------------------------------------------------------------
void Poller::FillActiveChannel(int active_nums, ChannelList* active_channel_list)
{
    assert(static_cast<size_t>(active_nums) <= event_list_.size());

    for(int i=0; i<active_nums; i++)
    {
        Channel* channel = static_cast<Channel*>(event_list_[i].data.ptr);
        channel->set_revents(event_list_[i].events);
        active_channel_list->push_back(channel);
    }

    return;
}
//---------------------------------------------------------------------------
static const char* OperatorToString(int op)
{
    switch(op)
    {
    case EPOLL_CTL_ADD:
        return "ADD";
    
    case EPOLL_CTL_DEL:
        return "DEL";

    case EPOLL_CTL_MOD:
        return "mod";

    default:
        assert(0);
        return "Unknown operator";
    }
}
//---------------------------------------------------------------------------
void Poller::Update(int op, Channel* channel)
{
    //统计当前监控的fd数量
    if(EPOLL_CTL_DEL == op) fd_nums_--;
    if(EPOLL_CTL_ADD == op) fd_nums_++;
    SystemLog_Debug("epoll_ctl op = %s, {fd:%d==>event:%s}", OperatorToString(op), channel->fd(), channel->EventsToString().c_str());

    struct epoll_event event;
    event.events    = channel->events();
    event.data.ptr  = channel;
    if(0 > ::epoll_ctl(efd_, op, channel->fd(), &event))
    {
        if(EPOLL_CTL_DEL == op) fd_nums_--;
        if(EPOLL_CTL_ADD == op) fd_nums_++;

        char buffer[128];
        SystemLog_Error("epoll_ctl error:errno:%d, msg:%s", errno, strerror_r(errno, buffer, 128));
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
}//namespace net
