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

class Epoller : public Poller
{
public:
    Epoller(EventLoop* event_loop);
    virtual ~Epoller();
    
    virtual uint64_t Poll(int timeoutS);

    virtual void UpdateChannel(Channel* channel);
    virtual void RemoveChannel(Channel* channel);

private:
    void FillActiveChannel(int active_nums);

    bool Update(int op, Channel* channel);

    void AddChannelListItem(Channel* channel);
    void DelChannelListItem(Channel* channel);

private:
    int efd_;
    std::vector<epoll_event> event_list_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //NET_EPOLLER_H_
