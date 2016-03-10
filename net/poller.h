//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_POLLER_H_
#define BASE_LINUX_NET_POLLER_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
#include "../base/timestamp.h"
//---------------------------------------------------------------------------
namespace net
{

class Channel;
struct epoll_event;

class Poller
{
public:
    typedef std::vector<Channel*> ChannelList;

    Poller();
    ~Poller();

    base::Timestamp Poll(int timeout, ChannelList* active_channle_list);    //timeout 以秒为单位

    void ChannelAdd(Channel* channel);
    void ChannelMod(Channel* channel);
    void ChannelDel(Channel* channel);

private:
    int         efd_;       //epoll fd
    uint64_t    fd_nums_;   //fd 个数

    std::vector<epoll_event> event_list_;   //监听的event fd数组
};

}//namespace net
//---------------------------------------------------------------------------
#endif// BASE_LINUX_NET_POLLER_H_
