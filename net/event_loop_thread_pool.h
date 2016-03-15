//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_EVENT_LOOP_THREAD_POOL_H_
#define BASE_LINUX_NET_EVENT_LOOP_THREAD_POOL_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
//---------------------------------------------------------------------------
namespace net
{

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool
{
public:
    EventLoopThreadPool(EventLoop* loop_main, const std::string& name="");
    ~EventLoopThreadPool();

    void set_thread_nums(int nums)  { thread_nums_ = nums; }
    
    void Start();
    void Stop();

    EventLoop* GetNextEventLoop();

    const std::string& name()   { return name_; }

private:
    EventLoop*  loop_main_;
    std::string name_;
    bool        running_;
    int         thread_nums_;
    int         next_;

    std::vector<std::shared_ptr<EventLoopThread>>   loop_threads_;
    std::vector<EventLoop*>                         loops_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //BASE_LINUX_NET_EVENT_LOOP_THREAD_POOL_H_
