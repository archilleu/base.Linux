//---------------------------------------------------------------------------
#include <assert.h>
#include "event_loop.h"
#include "event_loop_thread.h"
#include "net_logger.h"
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
EventLoopThread::EventLoopThread()
:   event_loop_(nullptr),
    running_(false),
    thread_(std::bind(&EventLoopThread::OnEventLoop, this), "event loop thread")
{
    NetLogger_trace("event loop thread ctor:%p", this);
}
//---------------------------------------------------------------------------
EventLoopThread::~EventLoopThread()
{
    NetLogger_trace("event loop thread dtor:%p", this);

    assert(false == running_);
    return;
}
//---------------------------------------------------------------------------
EventLoop* EventLoopThread::StartLoop()
{
    assert(false == running_);
    NetLogger_info("EventLoopThread(%p) start loop", this);

    if(false == thread_.Start())
    {
        NetLogger_off("event loop thread start failed");
        exit(errno);
    }

    {
        std::unique_lock<std::mutex> lock(mutex_);
        while(!event_loop_)
            cond_.wait(lock);
    }

    running_ = true;
    return event_loop_;
}
//---------------------------------------------------------------------------
void EventLoopThread::StopLoop()
{
    assert(true == running_);
    NetLogger_info("EventLoopThread(%p) stop loop", this);

    event_loop_->Quit();
    thread_.Join();
    running_ = false;
    event_loop_ = nullptr;

    return;
}
//---------------------------------------------------------------------------
void EventLoopThread::OnEventLoop()
{
    EventLoop event_loop;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        event_loop_ = &event_loop;
        cond_.notify_one();
    }

    event_loop.Loop();

    NetLogger_trace("EventLoopThread(%p) onEventLoop finished", this);
    return;
}
//---------------------------------------------------------------------------

}//namespace net
//---------------------------------------------------------------------------
