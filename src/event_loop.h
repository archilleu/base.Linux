//---------------------------------------------------------------------------
#ifndef NET_EVENT_LOOP_H_
#define NET_EVENT_LOOP_H_
//---------------------------------------------------------------------------
#include <atomic>
#include <list>
#include <mutex>
#include "callback.h"
#include "timer_task_id.h"
//---------------------------------------------------------------------------
namespace net
{

class Channel;
class Poller;
class TimerTaskQueue;

class EventLoop
{
public:
    using Task          = std::function<void (void)>;
    using SignalFunc    = std::function<void (void)>;
    using BeforLoopFunc = std::function<void (void)>;
    using AfterLoopFunc = std::function<void (void)>;

    EventLoop();
    ~EventLoop();
    EventLoop(const EventLoop&) =delete;
    EventLoop& operator=(const EventLoop&) =delete;

    void set_sig_int_callback(SignalFunc&& callback)     { sig_int_callback_    = std::move(callback); }
    void set_sig_quit_callback(SignalFunc&& callback)    { sig_quit_callback_   = std::move(callback); }
    void set_sig_usr1_callback(SignalFunc&& callback)    { sig_usr1_callback_   = std::move(callback); }
    void set_sig_usr2_callback(SignalFunc&& callback)    { sig_usr2_callback_   = std::move(callback); }

    void set_befor_function(BeforLoopFunc&& callback)   { befor_func_ = std::move(callback); }
    void set_after_function(AfterLoopFunc&& callback)   { after_func_ = std::move(callback); }

    //启动和退出
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
    TimerTaskId RunAt       (uint64_t when, CallbackTimerTask&& callback);
    TimerTaskId RunAfter    (int delayS, CallbackTimerTask&& callback);
    TimerTaskId RunInterval (int intervalS, CallbackTimerTask&& callback);
    void        RunCancel   (TimerTaskId timer_task_id);

public:
    static EventLoop* GetEventLoopOfCurrentThread();

private:
    friend class Channel;
    //改变监控的Channel状态,由connection通过Channel发起改变请求,Channel再通过EventLoop向poller请求改变
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

    //调试接口
    void PrintActiveChannels() const;

private:
    std::atomic<bool>   looping_;
    int                 tid_;
    const char*         tname_;
    int64_t             iteration_;
    
    //wakeup
    int wakeupfd_;
    std::shared_ptr<Channel> channel_wakeup_;

    //Task队列
    std::list<Task>     task_list_;
    std::mutex          mutex_;
    std::atomic<bool>   need_wakup_;

    std::shared_ptr<Poller> poller_;
    std::shared_ptr<TimerTaskQueue> timer_task_queue_;

    //signal
    int sig_fd_;
    SignalFunc  sig_int_callback_;
    SignalFunc  sig_quit_callback_;
    SignalFunc  sig_usr1_callback_;
    SignalFunc  sig_usr2_callback_;
    std::shared_ptr<Channel> channel_sig_;

    //befor after run
    BeforLoopFunc befor_func_;
    AfterLoopFunc after_func_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //NET_EVENT_LOOP_H_

