//---------------------------------------------------------------------------
#include <cassert>
#include "event_loop_thread_pool.h"
#include "event_loop_thread.h"
#include "event_loop.h"
#include "net_logger.h"
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
EventLoopThreadPool::EventLoopThreadPool(EventLoop* loop_main)
:   loop_main_(loop_main),
    running_(false),
    thread_nums_(0),
    next_(0)
{
    NetLogger_trace("EventLoopThreadPool(%p) ctor");
}
//---------------------------------------------------------------------------
EventLoopThreadPool::~EventLoopThreadPool()
{
    NetLogger_trace("EventLoopThreadPool(%p) dtor");

    if(running_)
        Stop();
}
//---------------------------------------------------------------------------
void EventLoopThreadPool::Start()
{
    loop_main_->AssertInLoopThread();
    assert(false == running_);
    NetLogger_info("EventLoopTreadPool(%p) start");

    for(int i=0; i<thread_nums_; i++)
    {
        std::shared_ptr<EventLoopThread> loop_thread(new EventLoopThread());

        EventLoop* loop = loop_thread->StartLoop();
        loop_threads_.push_back(loop_thread);
        loops_.push_back(loop);
    }

    running_ = true;
    return;
}
//---------------------------------------------------------------------------
void EventLoopThreadPool::Stop()
{
    loop_main_->AssertInLoopThread();
    NetLogger_info("EventLoopTreadPool(%p) stop");

    for(auto iter : loop_threads_)
        iter->StopLoop();

    running_ = false;
    loop_main_ = 0;
    return;
}
//---------------------------------------------------------------------------
EventLoop* EventLoopThreadPool::GetNextEventLoop()
{
    assert(true == running_);
    loop_main_->AssertInLoopThread();

    if(true == loops_.empty())
        return loop_main_;

    EventLoop* loop = loops_[next_++%thread_nums_];
    return loop;
}
//---------------------------------------------------------------------------

}//namespace net
