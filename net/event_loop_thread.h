//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_EVENT_LOOP_THREAD_H_
#define BASE_LINUX_NET_EVENT_LOOP_THREAD_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
#include "../base/thread.h"
//---------------------------------------------------------------------------
namespace net
{

class EventLoop;

class EventLoopThread
{
public:
    EventLoopThread();
    ~EventLoopThread();

    EventLoop*  StartLoop();
    void        StopLoop();

private:
    void OnThreadEventLoop();

private:
    EventLoop*  event_loop_;
    bool        running_;

    base::Thread            thread_;
    std::mutex              mutex_;
    std::condition_variable cond_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //BASE_LINUX_NET_EVENT_LOOP_THREAD_H_
