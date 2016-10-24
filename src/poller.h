//---------------------------------------------------------------------------
#ifndef NET_POLLER_H_
#define NET_POLLER_H_
//---------------------------------------------------------------------------
#include <vector>
#include "event_loop.h"
#include "../depend/base/include/timestamp.h"
//---------------------------------------------------------------------------
namespace net
{

class Channel;

class Poller
{
public:
    using ChannelList = std::vector<Channel*>;

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

    virtual uint64_t Poll(int timeoutS) =0;

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
    //在数组中记录所有的Channel，数组下标为Channel's fd
    ChannelList channels_;          

    //活跃的channel数组，因为该数组不会清空（避免反复分配内存），
    //所以当数组遇到第一个nullptr时表示当前活跃channel没有更多了
    ChannelList active_channels_;

    size_t      channel_num_;   //channel个数
    int         cur_max_fd_;    //当前最大fd
    size_t      egen_;          //poll出错事件次数

private:
    EventLoop* owner_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif// NET_POLLER_H_
