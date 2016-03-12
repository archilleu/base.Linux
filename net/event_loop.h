//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_EVENT_LOOP_H_
#define BASE_LINUX_NET_EVENT_LOOP_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
#include "poller.h"
#include "callback.h"
#include "timer_task.h"
//---------------------------------------------------------------------------
namespace net
{

class Channel;
class Poller;
class TimerTaskQueue;

class EventLoop
{
public:
    typedef std::function<void (void)> Task;

    EventLoop();
    ~EventLoop();

    void Loop();
    void Quit();

    void AssertInLoopThread();
    bool IsInLoopThread();

    //线程安全方法,如果调用着的线程是本EventLoop线程,则RunInLoop会立刻执行,否则排队到QueueInLoop
    void RunInLoop  (const Task& task);
    void QueueInLoop(const Task& task);

    //定时任务
    TimerTask::Ptr  RunAt       (const base::Timestamp when, const CallbackTimerTask& callback);
    TimerTask::Ptr  RunAfter    (int delayS, const CallbackTimerTask& callback);
    TimerTask::Ptr  RunInterval (int intervalS, const CallbackTimerTask& callback);
    void            RunCancel   (TimerTask::Ptr timer_task);

    //改变监控的Channel状态,一般由connection通过Channel发起改变请求,Channel再通过EventLoop向poller请求改变
    void ChannelAdd(Channel* channel);
    void ChannelMod(Channel* channel);
    void ChannelDel(Channel* channel);

private:
    void AbortNotInLoopThread();

    //当poll没有外在事件发生时,poll阻塞返回需要最长5s,QueueInLoop和RunInLoop也因此需要5s
    //为避免发生这样的情况,使用额外的手动事件来触发poll
    void Wakeup();
    void HandleRead();

    //处理RunInLoop和QueueInLoop的请求
    void DoPendingTasks();

private:
    bool        looping_;
    uint32_t    tid_;
    const char* tname_;
    
    //wakeup
    int                         wakeupfd_;
    std::shared_ptr<Channel>    channel_wakeup_;

    //Task队列
    std::list<Task> task_list_;
    std::mutex      mutex_;
    bool            is_pending_task_;


    Poller::ChannelList             active_channel_list_;
    std::shared_ptr<Poller>         poller_;
    std::shared_ptr<TimerTaskQueue> timer_task_queue_;

protected:
    DISALLOW_COPY_AND_ASSIGN(EventLoop);
};

}//namespace net
//---------------------------------------------------------------------------
#endif //BASE_LINUX_NET_EVENT_LOOP_H_

