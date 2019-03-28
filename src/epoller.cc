//---------------------------------------------------------------------------
#include <sys/epoll.h>
#include <cassert>
#include <unistd.h>
#include "epoller.h"
#include "channel.h"
#include "net_logger.h"
#include "../thirdpart/base/include/timestamp.h"
//---------------------------------------------------------------------------
namespace
{
const int kNew = 1;
const int kAdded = 2;
const int kDel = 3;

const int kInitActiveChannelSize = 512;
const int kInitTotalChannelSize = 1024*1024; //100w
}
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
Epoller::Epoller(EventLoop* event_loop)
:   Poller(event_loop)
{
    NetLogger_trace("Epoller ctor %p", this);

    efd_ = ::epoll_create1(EPOLL_CLOEXEC);
    if(0 > efd_)
    {
        NetLogger_off("create epoll fd failed");
        exit(-1);
    }

    this->channels_.resize(kInitTotalChannelSize);
    this->active_channels_.resize(kInitActiveChannelSize);
    event_list_.resize(kInitActiveChannelSize);

    return;
}
//---------------------------------------------------------------------------
Epoller::~Epoller()
{
    NetLogger_trace("Epoller dtor %p", this);
    assert(((void)"0 != channel_nums_", 0==channel_nums_));

    close(efd_);
    return;
}
//---------------------------------------------------------------------------
uint64_t Epoller::Poll(int timeoutS)
{
    this->AssertInLoopThread();

    this->active_channels_[0] = nullptr;

    int nums = ::epoll_wait(efd_, static_cast<struct epoll_event*>(event_list_.data()),
            static_cast<int>(event_list_.size()), timeoutS*1000);
    uint64_t now = base::Timestamp::Now().Microseconds();
    if(0 < nums)
    {
        NetLogger_trace("event nums: %d", nums);

        if(nums == static_cast<int>(event_list_.size()))
        {
            //达到最大的事件接收阈值，表示繁忙，扩大接收事件阈值
            size_t size = event_list_.size();
            event_list_.resize(size*2);

            this->active_channels_.clear(); //避免多余的拷贝
            this->active_channels_.resize(size*2);
        }

        FillActiveChannel(nums);
    }
    else if(0 == nums)
    {
        //没有事件
        NetLogger_trace("no events");
    }
    else
    {
        //出错
        NetLogger_error("epoll_wait error:no:%d, msg:%s", errno, OSError(errno));
    }

    return now;
}
//---------------------------------------------------------------------------
static const char* StatusToString(int status)
{
    switch(status)
    {
        case kNew:
            return "NEW";

        case kAdded:
            return "ADDED";

        case kDel:
            return "DEL";

        default:
            return "INVALID";
    }
}
//---------------------------------------------------------------------------
void Epoller::UpdateChannel(Channel* channel)
{
    this->AssertInLoopThread();

    int fd = channel->fd();
    int status = channel->stauts();
    NetLogger_trace("fd:%d, events:%s, status:%s", fd, channel->EventsToString_().c_str(),
            StatusToString(status));

    //当前fd下标大于channels_可容纳的大小
    //考虑是否和TCPServer一样的扩容机制，目前不需要扩容
    /*
    if(fd >= static_cast<int>(channels_.size()))
    {
        channels_.resize(channels_.size()*2);
    }
    */

    switch(status)
    {
        case kNew:
            //检擦是已经存在该fd，正常情况下该状态fd是不应该存在的
            if(nullptr != channels_[fd])
            {
                assert(0);
                NetLogger_error("channel(fd:%d) already add", fd);
                break;
            }

            if(!channel->IsNoneEvent()) //需要监控该fd
            {
                if(false == Update(EPOLL_CTL_ADD, channel))
                {
                    NetLogger_error("Update EPOLL_CTL_ADD error, fd:%d", fd);
                    return;
                }
                channel->set_status(kAdded);
            }
            else    //只是添加到channel_中，但是未有感兴趣的事件
            {
                channel->set_status(kDel);
            }

            AddChannelListItem(channel);
            break;

        case kAdded:
            if(channel != channels_[fd])
            {
                assert(0);
                NetLogger_error("channel(%p)  channels_[fd](%p), fd:%d", 
                        channel, channels_[fd], fd);
                return;
            }

            if(!channel->IsNoneEvent())
            {
                if(false == Update(EPOLL_CTL_MOD, channel))
                {
                    NetLogger_error("Update EPOLL_CTL_MOD error, fd:%d", fd);
                    return;
                }
            }
            else
            {
                if(false == Update(EPOLL_CTL_DEL, channel))
                {
                    NetLogger_error("Update EPOLL_CTL_DEL error, fd:%d", fd);
                    return;
                }
                channel->set_status(kDel);
            }
            break;

        case kDel:
            if(channel != channels_[fd])
            {
                assert(0);
                NetLogger_error("channel(%p)  channels_[fd](%p), fd:%d", 
                        channel, channels_[fd], fd);
                return;
            }

            if(!channel->IsNoneEvent())
            {
                if(false == Update(EPOLL_CTL_ADD, channel))
                {
                    NetLogger_error("Update EPOLL_CTL_ADD error, fd:%d", fd);
                    return;
                }
                channel->set_status(kAdded);
            }
            break;

        default:
            assert(((void)"invalid status", 0));
    }

    return;
}
//---------------------------------------------------------------------------
void Epoller::RemoveChannel(Channel* channel)
{
    this->AssertInLoopThread();

    int fd = channel->fd();
    int status = channel->stauts();
    NetLogger_trace("fd:%d, events:%s, status:%s", fd, channel->EventsToString_().c_str(),
            StatusToString(status));

    if(kNew != status)
    {
        assert(((void)"channel != channels_[fd]",channel == this->channels_[fd]));

        if(!channel->IsNoneEvent())
        {
            assert(kAdded == status);
            if(false == Update(EPOLL_CTL_DEL, channel))
            {
                NetLogger_error("Update EPOLL_CTL_DEL error, fd:%d", fd);
                return;
            }
        }
        else
        {
            assert(kDel == status);
        }

        channel->set_status(kNew);
        DelChannelListItem(channel);
    }

    return;
}
//---------------------------------------------------------------------------
void Epoller::FillActiveChannel(int active_nums)
{
    int i = 0;
    for(; i<active_nums; i++)
    {
        Channel* channel = static_cast<Channel*>(event_list_[i].data.ptr);
        if(!channel->IsNoneEvent())
        {
            channel->set_revents(event_list_[i].events);
            this->active_channels_[i] = channel;
        }
        else
        {
            NetLogger_warn("spurious notification");
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
            assert(((void)"unknow operator", 0));
            return "Unknown operator";
    }
}
//---------------------------------------------------------------------------
bool Epoller::Update(int op, Channel* channel)
{
    NetLogger_trace("epoll_ctl op = %s, {fd:%d==>event:%s}", 
            OperatorToString(op), channel->fd(), channel->EventsToString_().c_str());

    struct epoll_event event;
    event.events = channel->events();
    event.data.ptr  = channel;
    if(0 > ::epoll_ctl(efd_, op, channel->fd(), &event))
    {
        NetLogger_error("epoll_ctl error: op=%s, fd=%d, errno:%d, msg:%s", 
                OperatorToString(op), channel->fd(), errno, OSError(errno));
        assert(((void)"epoll_ctl error", 0));
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void Epoller::AddChannelListItem(Channel* channel)
{
    int fd = channel->fd();
    this->channels_[fd] = channel;
    this->channel_nums_++;

    if(this->cur_max_fd_ < fd)
        this->cur_max_fd_ = fd;

#ifdef _DEBUG
    this->DumpChannels();
#endif
    return;
}
//---------------------------------------------------------------------------
void Epoller::DelChannelListItem(Channel* channel)
{
    int fd = channel->fd();
    this->channels_[fd] = nullptr;
    this->channel_nums_--;

    if(this->cur_max_fd_ == fd)
    {
        int i = fd - 1;
        for(; i>0; i--)
        {
            if(nullptr == channels_[i])
                continue;

            break;
        }
        this->cur_max_fd_ = i;
    }

#ifdef _DEBUG
    this->DumpChannels();
#endif
    return;
}
//---------------------------------------------------------------------------

}//namespace net
//---------------------------------------------------------------------------
