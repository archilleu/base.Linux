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
base::Timestamp Poller::Poll(int timeout, ChannelList* active_channle_list)
{
}
//---------------------------------------------------------------------------
void Poller::~ChannelAdd(Channel* channel)
{
}
//---------------------------------------------------------------------------
void Poller::~ChannelMod(Channel* channel)
{
}
//---------------------------------------------------------------------------
void Poller::~ChannelDel(Channel* channel)
{
}
//---------------------------------------------------------------------------
}//namespace net
