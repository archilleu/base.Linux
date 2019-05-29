//---------------------------------------------------------------------------
#ifndef NET_EVENT_LOOP_THREAD_H_ 
#define NET_EVENT_LOOP_THREAD_H_ 
//---------------------------------------------------------------------------
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "base/include/thread.h"
#include "base/include/noncopyable.h"
//---------------------------------------------------------------------------
namespace net
{

class EventLoop;

class EventLoopThread : public base::Noncopyable
{
public:
    EventLoopThread();
    ~EventLoopThread();

public:
    EventLoop* StartLoop();

    //不能在当前event loop中调用（线程异常）
    void StopLoop();

private:
    void OnEventLoop();

private:
    EventLoop* event_loop_;
    std::atomic<bool> running_;

    base::Thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //NET_EVENT_LOOP_THREAD_H_

