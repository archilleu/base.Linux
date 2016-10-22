//---------------------------------------------------------------------------
#include <cassert>
#include "event_loop_thread.h"
#include "event_loop.h"
#include "net_log.h"
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
EventLoopThread::EventLoopThread()
:   event_loop_(0),
    running_(false),
    thread_(std::bind(&EventLoopThread::OnThreadEventLoop, this), "event loop thread")
{
}
//---------------------------------------------------------------------------
EventLoopThread::~EventLoopThread()
{
    if(true == running_)
        thread_.Join();

    return;
}
//---------------------------------------------------------------------------
EventLoop* EventLoopThread::StartLoop()
{
    assert(false == running_);

    if(false == thread_.Start())
    {
        SystemLog_Error("event loop start failed");
        abort();
    }

    std::unique_lock<std::mutex> lock(mutex_);
    while(0 == event_loop_)
        cond_.wait(lock);

    running_ = true;
    return event_loop_;
}
//---------------------------------------------------------------------------
void EventLoopThread::StopLoop()
{
    if(false == running_)
        return;

    event_loop_->Quit();
    thread_.Join();
    running_    = false;
    event_loop_ = 0;

    return;
}
//---------------------------------------------------------------------------
void EventLoopThread::OnThreadEventLoop()
{
    EventLoop event_loop;

    {
    std::unique_lock<std::mutex> lock(mutex_);
    event_loop_ = &event_loop;
    cond_.notify_one();
    }

    //进入事件循环
    event_loop.Loop();

    event_loop_ = 0;
    return;
}
//---------------------------------------------------------------------------

}//namespace net
