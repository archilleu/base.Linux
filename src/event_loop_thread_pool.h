//---------------------------------------------------------------------------
#ifndef NET_EVENT_LOOP_THREAD_POOL_H_
#define NET_EVENT_LOOP_THREAD_POOL_H_
//---------------------------------------------------------------------------
#include <string>
#include <vector>
#include <memory>
#include "base/include/noncopyable.h"
//---------------------------------------------------------------------------
namespace net
{

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : public base::Noncopyable
{
public:
    EventLoopThreadPool(EventLoop* loop_main);
    ~EventLoopThreadPool();

    void set_thread_nums(int nums)  { thread_nums_ = nums; }
    
    void Start();
    void Stop();

    EventLoop* GetNextEventLoop();

private:
    EventLoop* loop_main_;
    bool running_;
    int thread_nums_;
    int next_;

    std::vector<std::shared_ptr<EventLoopThread>> loop_threads_;
    std::vector<EventLoop*> loops_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //NET_EVENT_LOOP_THREAD_POOL_H_
