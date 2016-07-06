//---------------------------------------------------------------------------
#include "poller.h"
#include "channel.h"
#include "net_log.h"
//---------------------------------------------------------------------------
namespace net
{
//---------------------------------------------------------------------------
bool Poller::HasChannel(Channel* channel)
{
    AssertInLoopThread();

    int fd = channel->fd();
    assert(((void)"fd > channels_.size()", fd <= static_cast<int>(channels_.size())));

    Channel* _channel = channels_[fd];
    if(nullptr == _channel)
        return false;
    assert(((void)"channel no equal channel", _channel == channel));

    return true;
}
//---------------------------------------------------------------------------
void Poller::DumpChannel()
{
    AssertInLoopThread();

    size_t count = 0;
    for(size_t i=0; i<channels_.size(); i++)
    {
        if(nullptr == channels_[i])
            continue;

        assert(((void)"channels fd no match idx", channels_[i]->fd() == static_cast<int>(i)));
        count++;
    }

    assert(((void)"channels num not eq num", count == channel_num_));

    return;
}
//---------------------------------------------------------------------------
}//namespace net
