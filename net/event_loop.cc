//---------------------------------------------------------------------------
#include "event_loop.h"
#include "net_log.h"
#include <poll.h>
//---------------------------------------------------------------------------
namespace net
{
//---------------------------------------------------------------------------
EventLoop::EventLoop()
:   looping_(false),
    tid_(base::CurrentThread::Tid()),
    tname_(base::CurrentThread::ThreadName())
{
}
//---------------------------------------------------------------------------
EventLoop::~EventLoop()
{
    Quit();
}
//---------------------------------------------------------------------------
void EventLoop::Loop()
{
    assert(!looping_);
    AssertInLoopThread();

    looping_ = true;

    SystemLog_Info("%p Event loop start", this);

    ::poll(NULL, 0, 5*1000);
    
    SystemLog_Info("%p Event loop stop", this);
    return;
}
//---------------------------------------------------------------------------
void EventLoop::Quit()
{
}
//---------------------------------------------------------------------------
void EventLoop::AssertInLoopThread()
{
    if(!IsInLoopThread())
    {
        AbortNotInLoopThread();
    }

    return;
}
//---------------------------------------------------------------------------
bool EventLoop::IsInLoopThread()
{
    return (tid_ == base::CurrentThread::Tid());
}
//---------------------------------------------------------------------------
void EventLoop::AbortNotInLoopThread()
{
    SystemLog_Debug("%p was create in tid:%u, tname:%s, but current tid:%u, tname:%s",
                    this, tid_, tname_, base::CurrentThread::Tid(), base::CurrentThread::ThreadName());

    assert(0);
    return;
}
//---------------------------------------------------------------------------
}//namespace net
