
//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_POLLER_H_
#define BASE_LINUX_NET_POLLER_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
#include "../base/timestamp.h"
#include "event_loop.h"
//---------------------------------------------------------------------------
namespace net
{

class Channel;

class Poller
{
public:
    typedef std::vector<Channel*> ChannelList;

    Poller(EventLoop* owner)
    :   channel_num_(0),
        cur_max_fd_(0),
        egen_(0),
        owner_(owner)
    {
    }
    virtual ~Poller()
    {
    }

    virtual base::Timestamp Poll(int timeoutS) =0;
    const ChannelList& active_channels() const { return active_channels_; }

    virtual void ChannelUpdate(Channel* channel) =0;
    virtual void ChannelRemove(Channel* channel) =0;

    bool HasChannel(Channel* channel) const;

    void AssertInLoopThread() const  { owner_->AssertInLoopThread(); }

public:
    static Poller* NewDefaultPoller(EventLoop* loop);

protected:
    void DumpChannel();

protected:
    //由底层事件机制决定该数组大小
    //在数组中记录所有的Channel，数组下标为Channel fd
    ChannelList channels_;          

    //活跃的channel数组，因为该数组不会清空（避免反复分配内存），
    //所以当数组遇到第一个nullptr时表示当前活跃channel没有更多了
    //由底层事件机制决定该数组大小
    ChannelList active_channels_;

    size_t      channel_num_;
    int         cur_max_fd_;//current max fd
    size_t      egen_;

private:
    EventLoop* owner_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif// BASE_LINUX_NET_POLLER_H_
