//---------------------------------------------------------------------------
#ifndef NET_POLLER_H_
#define NET_POLLER_H_
//---------------------------------------------------------------------------
#include <vector>
#include <map>
#include "../thirdpart/base/include/noncopyable.h"
#include "event_loop.h"
//---------------------------------------------------------------------------
namespace net
{

class Channel;

class Poller : public base::Noncopyable
{
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop* event_loop)
    :   channel_nums_(0),
        cur_max_fd_(0),
        egen_(0),
        event_loop_(event_loop)
    {
    }
    virtual ~Poller()
    {
    }

    virtual uint64_t Poll(int timeoutS) =0;
    
    const ChannelList& active_channels() const { return active_channels_; }

    virtual void UpdateChannel(Channel* channel) =0;
    virtual void RemoveChannel(Channel* channel) =0;

    bool HasChannel(Channel* channel) const;

    void AssertInLoopThread() const { event_loop_->AssertInLoopThread(); };

public:
    static Poller* NewDefaultPoller(EventLoop* event_loop);

protected:
    void DumpChannels() const;

protected:
    //在数组中记录所有的channel，数组下标==channel的fd，因为linux
    //按照当前最大可用fd来分配fd，这样查找channel可以使用数组下标
    //达到常量级别的查找速度
    ChannelList channels_;

    //活跃的channel数组，该数组不会清空（避免反复分配内存）,所以当数
    //组遇到第一个nullptr时表示当前没有更多的活跃channel
    ChannelList active_channels_;

    size_t channel_nums_;   //channel 总个数
    int cur_max_fd_;        //当前最大fd

    size_t egen_;           //poll出错次数

private:
    EventLoop* event_loop_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif // NET_POLLER_H_
