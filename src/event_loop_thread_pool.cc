//---------------------------------------------------------------------------
#include <cassert>
#include "event_loop_thread_pool.h"
#include "event_loop_thread.h"
#include "event_loop.h"
#include "net_log.h"
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
EventLoopThreadPool::EventLoopThreadPool(EventLoop* loop_main, const std::string& pool_name)
:   loop_main_(loop_main),
    name_(pool_name),
    running_(false),
    thread_nums_(0),
    next_(0)
{
}
//---------------------------------------------------------------------------
EventLoopThreadPool::~EventLoopThreadPool()
{
    Stop();
}
//---------------------------------------------------------------------------
void EventLoopThreadPool::Start()
{
    if(running_)
        return;

    loop_main_->AssertInLoopThread();

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
    if(false == running_)
        return;

    for(auto iter : loop_threads_)
        iter->StopLoop();

    running_    = false;
    loop_main_  = 0;
    return;
}
//---------------------------------------------------------------------------
EventLoop* EventLoopThreadPool::GetNextEventLoop()
{
    assert(running_);

    loop_main_->AssertInLoopThread();

    if(true == loops_.empty())
        return loop_main_;

    EventLoop* loop = loops_[next_++%thread_nums_];
    return loop;
}
//---------------------------------------------------------------------------

}//namespace net
