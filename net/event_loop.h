//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_EVENT_LOOP_H_
#define BASE_LINUX_NET_EVENT_LOOP_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
#include "callback.h"
#include "timer_task_id.h"
#include <atomic>
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
    typedef std::function<void (void)> SignalFunc;

    EventLoop();
    ~EventLoop();

    void set_sig_int_callback(SignalFunc&& callback)     { sig_int_callback_    = std::move(callback); }
    void set_sig_quit_callback(SignalFunc&& callback)    { sig_quit_callback_   = std::move(callback); }
    void set_sig_usr1_callback(SignalFunc&& callback)    { sig_usr1_callback_   = std::move(callback); }
    void set_sig_usr2_callback(SignalFunc&& callback)    { sig_usr2_callback_   = std::move(callback); }

    void Loop();
    void Quit();

    void SetAsSignalHandleEventLoop();

    uint64_t iteration() const { return iteration_; }

    void AssertInLoopThread() const;
    bool IsInLoopThread() const;

    //线程安全方法,如果调用着的线程是本EventLoop线程,则RunInLoop会立刻执行,否则排队到QueueInLoop
    void RunInLoop  (Task&& task);
    void QueueInLoop(Task&& task);

    //定时任务
    TimerTaskId RunAt       (base::Timestamp when, CallbackTimerTask&& callback);
    TimerTaskId RunAfter    (int delayS, CallbackTimerTask&& callback);
    TimerTaskId RunInterval (int intervalS, CallbackTimerTask&& callback);
    void        RunCancel   (TimerTaskId timer_task_id);

public:
    static EventLoop* GetEventLoopOfCurrentThread();

public:
    //改变监控的Channel状态,一般由connection通过Channel发起改变请求,Channel再通过EventLoop向poller请求改变,只由内部类调用
    void ChannelUpdate(Channel* channel);
    void ChannelRemove(Channel* channel);
    bool HasChannel(Channel* channel) const;

private:
    void AbortNotInLoopThread() const;

    //当poll没有外在事件发生时,poll阻塞返回需要最长5s,QueueInLoop和RunInLoop也因此需要5s
    //为避免发生这样的情况,使用额外的手动事件来触发poll
    void Wakeup();
    void HandleWakeup();
    void HandleSignal();

    //处理RunInLoop和QueueInLoop的请求
    void DoPendingTasks();

    void PrintActiveChannels() const;

private:
    std::atomic<bool>   looping_;
    int                 tid_;
    const char*         tname_;
    int64_t             iteration_;
    
    //wakeup
    int                         wakeupfd_;
    std::shared_ptr<Channel>    channel_wakeup_;

    //Task队列
    std::list<Task>     task_list_;
    std::mutex          mutex_;
    std::atomic<bool>   need_wakup_;

    std::shared_ptr<Poller>         poller_;
    std::shared_ptr<TimerTaskQueue> timer_task_queue_;

    //signal
    int sig_fd_;
    SignalFunc  sig_int_callback_;
    SignalFunc  sig_quit_callback_;
    SignalFunc  sig_usr1_callback_;
    SignalFunc  sig_usr2_callback_;
    std::shared_ptr<Channel> channel_sig_;

protected:
    DISALLOW_COPY_AND_ASSIGN(EventLoop);
};

}//namespace net
//---------------------------------------------------------------------------
#endif //BASE_LINUX_NET_EVENT_LOOP_H_

