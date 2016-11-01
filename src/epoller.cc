//---------------------------------------------------------------------------
#include <sys/epoll.h>
#include <cassert>
#include <unistd.h>
#include "epoller.h"
#include "channel.h"
#include "net_log.h"
//---------------------------------------------------------------------------
namespace
{
    const int kNew  = 1;
    const int kAdded= 2;
    const int kDel  = 3;

    const int kInitActiveChannelSize = 512;
    const int kInitTotalChannelSize = 1024*1024; //100w
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
        NetLogger_off("create epoll fd failed");
        abort();
    }
    egen_ = 0;

    this->channels_.resize(kInitTotalChannelSize);
    this->active_channels_.resize(kInitActiveChannelSize);
    event_list_.resize(kInitActiveChannelSize);

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
uint64_t EPoller::Poll(int timeoutS)
{
    this->AssertInLoopThread();

    this->active_channels_[0] = nullptr;

    int nums = ::epoll_wait(efd_, static_cast<struct epoll_event*>(event_list_.data()), static_cast<int>(event_list_.size()), timeoutS*1000);
    uint64_t rcv_time = base::Timestamp::Now().Microseconds();
    
    //有事件
    if(0 < nums)
    {
        NetLogger_trace("event nums:%d", nums);

        if(nums == static_cast<int>(event_list_.size()))
        {
            size_t size = event_list_.size();
            event_list_.resize(size*2);

            this->active_channels_.clear();//避免多余的拷贝
            this->active_channels_.resize(size*2);
        }

        FillActiveChannel(nums);
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
        NetLogger_warn("epoll_wait error:no:%d, msg:%s", errno, OSError(errno));
        assert(((void)"epoll_wait error", 0));
    }

    return rcv_time;
}
//---------------------------------------------------------------------------
void EPoller::ChannelUpdate(Channel* channel)
{
    this->AssertInLoopThread();

    int fd      = channel->fd();
    int status  = channel->status();
    NetLogger_trace("fd:%d events:%d status:%d", fd, channel->events(), status);

    switch(status)
    {
        case kNew:
            assert(((void)"channel alreeady add", nullptr == this->channels_[fd]));
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

            AddfdList(channel);

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
    NetLogger_trace("fd:%d events:%d index:%d", fd, channel->events(), status);

    if(kNew != status)
    {
        assert(((void)"idx must <= channels_'s size", fd <= static_cast<int>(this->channels_.size())));
        assert(channel == this->channels_[fd]);
        
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
        DelfdList(channel);
    }

    return;
}
//---------------------------------------------------------------------------
void EPoller::FillActiveChannel(int active_nums)
{
    int i = 0;
    for(; i<active_nums; i++)
    {
        Channel* channel = static_cast<Channel*>(event_list_[i].data.ptr);
        assert(((void)"channels_ no equal channel", channel == this->channels_[channel->fd()]));
        if(!channel->IsNoneEvent())
        {
            channel->set_revents(event_list_[i].events);
            this->active_channels_[i] = channel;
        }
        else
        {
            assert(((void)"spurious notification", 0));
            this->egen_++;
        }
    }

    this->active_channels_[i] = nullptr;

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
    NetLogger_trace("epoll_ctl op = %s, {fd:%d==>event:%s}", OperatorToString(op), channel->fd(), channel->EventsToString().c_str());

    struct epoll_event event;
    event.events    = channel->events();
    event.data.ptr  = channel;
    if(0 > ::epoll_ctl(efd_, op, channel->fd(), &event))
    {
        NetLogger_error("epoll_ctl error: op=%s, fd=%d, errno:%d, msg:%s", OperatorToString(op), channel->fd(), errno, OSError(errno));
        assert(0);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void EPoller::AddfdList(Channel* channel)
{
    int fd = channel->fd();
    this->channels_[fd] = channel;
    channel_num_++;

    if(this->cur_max_fd_ < fd)
        this->cur_max_fd_ = fd;

#ifdef _DEBUG
    this->DumpChannel();
#endif

    return;
}
//---------------------------------------------------------------------------
void EPoller::DelfdList(Channel* channel)
{
    int fd = channel->fd();
    this->channels_[fd] = nullptr;
    channel_num_--;

    if(fd == this->cur_max_fd_)
    {
        for(int i=fd-1; i>=0; i--)
        {
            if(nullptr == this->channels_[i])
                continue;

            this->cur_max_fd_ = i;
            break;
        }
    }

#ifdef _DEBUG
    this->DumpChannel();
#endif

    return;
}
//---------------------------------------------------------------------------
}//namespace net
