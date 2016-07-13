//---------------------------------------------------------------------------
#include "epoller.h"
#include "channel.h"
#include "net_log.h"
#include <sys/epoll.h>
//---------------------------------------------------------------------------
namespace
{
    const int kNew  = 1;
    const int kAdded= 2;
    const int kDel  = 3;
}
//---------------------------------------------------------------------------
namespace net
{
//---------------------------------------------------------------------------
EPoller::EPoller(EventLoop* owner)
:   Poller(owner)
{
    efd_ = ::epoll_create1(EPOLL_CLOEXEC);
    if(0 > efd_)
    {
        SystemLog_Error("create epoll fd failed");
        abort();
    }
    egen_ = 0;

    event_list_.resize(128);
    return;
}
//---------------------------------------------------------------------------
EPoller::~EPoller()
{
    assert(0 == channel_num_);

    close(efd_);
    return;
}
//---------------------------------------------------------------------------
base::Timestamp EPoller::Poll(int timeoutS, ChannelList* active_channel_list)
{
    this->AssertInLoopThread();

    int nums = ::epoll_wait(efd_, static_cast<struct epoll_event*>(event_list_.data()), static_cast<int>(event_list_.size()), timeoutS*1000);
    base::Timestamp rcv_time = base::Timestamp::Now();
    
    //有事件
    if(0 < nums)
    {
        SystemLog_Debug("event nums:%d", nums);

        FillActiveChannel(nums, active_channel_list);

        if(nums == static_cast<int>(event_list_.size()))
            event_list_.resize(event_list_.size()*2);

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
        SystemLog_Error("epoll_wait error:no:%d, msg:%s", errno, StrError(errno));
        assert(((void)"epoll_wait error", 0));
    }

#ifdef _DEBUG
    this->DumpChannel();
#endif

    return rcv_time;
}
//---------------------------------------------------------------------------
void EPoller::ChannelUpdate(Channel* channel)
{
    this->AssertInLoopThread();

    int fd      = channel->fd();
    int status  = channel->status();
    SystemLog_Debug("fd:%d events:%d status:%d", fd, channel->events(), status);

    switch(status)
    {
        case kNew:
            assert(((void)"channel alreeady add", nullptr == channels_[fd]));
            if(!channel->IsNoneEvent())
            {
                if(false == Update(EPOLL_CTL_ADD, channel))
                    return;

                channel->set_status(kAdded);
            }
            else
            {
                channel->set_status(kDel);
            }

            AddfdList(fd, channel);

            break;

        case kAdded:
            assert(((void)"channel no eq channels_", channel == this->channels_[channel->fd()]));
            if(!channel->IsNoneEvent())
            {
                Update(EPOLL_CTL_MOD, channel);
            }
            else
            {
                Update(EPOLL_CTL_DEL, channel);
                channel->set_status(kDel);
            }

            break;
        
        case kDel:
            assert(((void)"channel no eq channels_", channel == this->channels_[channel->fd()]));

            if(!channel->IsNoneEvent())
            {
                Update(EPOLL_CTL_ADD, channel);
                channel->set_status(kAdded);
            }

            break;

        default:
            assert(((void)"status invalid", 0));
    }

    return;
}
//---------------------------------------------------------------------------
void EPoller::ChannelRemove(Channel* channel)
{
    this->AssertInLoopThread();
    int fd      = channel->fd();
    int status  = channel->status();
    SystemLog_Debug("fd:%d events:%d index:%d", fd, channel->events(), status);

    if(kNew != status)
    {
        assert(((void)"idx must <= channels_'s size", fd <= static_cast<int>(channels_.size())));
        assert(channel == channels_[fd]);
        
        if(!channel->IsNoneEvent())
        {
            assert(kAdded == status);
            if(false == Update(EPOLL_CTL_DEL, channel))
                return;
        }
        else
        {
            assert(kDel == status);
        }

        channel->set_status(kNew);
        DeldList(channel);
    }

    return;
}
//---------------------------------------------------------------------------
void EPoller::FillActiveChannel(int active_nums, ChannelList* active_channel_list)
{
    if(active_nums >= static_cast<int>(active_channel_list->size()))
        active_channel_list->resize(active_channel_list->size()*2);

    int i = 0;
    for(; i<active_nums; i++)
    {
        Channel* channel = static_cast<Channel*>(event_list_[i].data.ptr);
        assert(((void)"channels_ no equal channel", channel == this->channels_[channel->fd()]));
        if(!channel->IsNoneEvent())
        {
            channel->set_revents(event_list_[i].events);
            (*active_channel_list)[i] = channel;
        }
        else
        {
            assert(((void)"spurious notification", 0));
            this->egen_++;
        }
    }

    (*active_channel_list)[i] = nullptr;

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
        return "MOD";

    default:
        assert(0);
        return "Unknown operator";
    }
}
//---------------------------------------------------------------------------
bool EPoller::Update(int op, Channel* channel)
{
    SystemLog_Debug("epoll_ctl op = %s, {fd:%d==>event:%s}", OperatorToString(op), channel->fd(), channel->EventsToString().c_str());

    struct epoll_event event;
    event.events    = channel->events();
    event.data.ptr  = channel;
    if(0 > ::epoll_ctl(efd_, op, channel->fd(), &event))
    {
        SystemLog_Error("epoll_ctl error: op=%s, fd=%d, errno:%d, msg:%s", OperatorToString(op), channel->fd(), errno, StrError(errno));
        assert(0);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void EPoller::AddfdList(int fd, Channel* channel)
{
    if(fd >= static_cast<int>(this->channels_.size()))
        this->channels_.resize(channels_.size()*2);

    channels_[fd] = channel;
    channel_num_++;

    return;
}
//---------------------------------------------------------------------------
void EPoller::DeldList(Channel* channel)
{
    channels_[channel->fd()] = nullptr;
    channel_num_--;
}
//---------------------------------------------------------------------------
}//namespace net
