
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
        owner_(owner)
    {
        channels_.resize(base::UNIT_MB);
    }
    virtual ~Poller()
    {
    }

    virtual base::Timestamp Poll(int timeoutS, ChannelList* active_channel_list) =0;

    virtual void ChannelUpdate(Channel* channel) =0;
    virtual void ChannelRemove(Channel* channel) =0;

    bool HasChannel(Channel* channel) const;

    void AssertInLoopThread() const  { owner_->AssertInLoopThread(); }

public:
    static Poller* NewDefaultPoller(EventLoop* loop);

protected:
    void DumpChannel();

protected:
    //在数组中记录所有的Channel，数组下标为Channel fd
    ChannelList channels_;
    size_t      channel_num_;

private:
    EventLoop* owner_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif// BASE_LINUX_NET_POLLER_H_
