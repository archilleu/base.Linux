//---------------------------------------------------------------------------
#ifndef NET_EPOLLER_H_
#define NET_EPOLLER_H_
//---------------------------------------------------------------------------
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

    virtual uint64_t Poll(int timeoutS);

    virtual void ChannelUpdate(Channel* channel);
    virtual void ChannelRemove(Channel* channel);

private:
    void FillActiveChannel(int active_nums);

    bool Update(int op, Channel* channel);

    void AddfdList(Channel* channel);
    void DelfdList(Channel* channel);

private:
    int efd_;

    std::vector<epoll_event> event_list_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif// NET_EPOLLER_H_
