//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_EPOLLER_H_
#define BASE_LINUX_NET_EPOLLER_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
#include "poller.h"
//---------------------------------------------------------------------------
struct epoll_event;
//---------------------------------------------------------------------------
namespace net
{

class EPoller : public Poller
{
public:
    EPoller(EventLoop* owner);
    virtual ~EPoller();

    virtual base::Timestamp Poll(int timeoutS, ChannelList* active_channel_list);

    virtual void ChannelUpdate(Channel* channel);
    virtual void ChannelRemove(Channel* channel);

private:
    void FillActiveChannel(int active_nums, ChannelList* active_channel_list);

    bool Update(int op, Channel* channel);

    void AddfdList(int fd, Channel* channel);
    void DeldList(Channel* channel);

private:
    int efd_;

    std::vector<epoll_event> event_list_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif// BASE_LINUX_NET_EPOLLER_H_
