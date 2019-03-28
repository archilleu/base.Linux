//---------------------------------------------------------------------------
#include <cassert>
#include "poller.h"
#include "channel.h"
#include "net_logger.h"
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
bool Poller::HasChannel(Channel* channel) const
{
    AssertInLoopThread();

    int fd = channel->fd();
    if(fd >= static_cast<int>(channels_.size()))
    {
        assert(((void)"fd > channels_.size()", false));
        return false;
    }

    Channel* _channel = channels_[fd];
    if(nullptr == _channel)
        return false;

    assert(((void)"channel no equal channel", _channel == channel));
    return true;
}

//---------------------------------------------------------------------------
void Poller::DumpChannels() const
{
    AssertInLoopThread();

    assert(((void)"current max fd >= channels_.size()",
                static_cast<size_t>(cur_max_fd_) < channels_.size()));

    size_t count = 0;
    for(int i=0; i<=cur_max_fd_; i++)
    {
        if(nullptr == channels_[i])
            continue;

        assert(((void)"channels fd no match idx", channels_[i]->fd() == i));
        count++;
    }

    assert(((void)"channels num not eq count", count == channel_nums_));
    NetLogger_trace("has channels:%zu, current max fd:%d, egen:%zu", count, cur_max_fd_, egen_); 

    return;
}
//---------------------------------------------------------------------------

}//namespace net
