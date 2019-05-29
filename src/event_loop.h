//---------------------------------------------------------------------------
#ifndef NET_EVENT_LOOP_H_
#define NET_EVENT_LOOP_H_
//---------------------------------------------------------------------------
#include <vector>
#include <list>
#include <mutex>
#include "base/include/noncopyable.h"
#include "base/include/thread.h"
#include "base/include/timestamp.h"
#include "base/include/logger.h"
#include "callback.h"
//---------------------------------------------------------------------------
namespace net
{

class Channel;
class Poller;
class TimerId;
class TimerQueue;

class EventLoop : public base::Noncopyable
{
public:
    using Task = std::function<void (void)>;
    using SignalFunc = std::function<void (void)>;
    using BeforLoopFunc = std::function<void (void)>;
    using AfterLoopFunc = std::function<void (void)>;

    EventLoop();
    ~EventLoop();

public:
    void Loop();
    void Quit();

    //信号捕获
    void set_sig_int_cb(SignalFunc&& cb) { sig_int_cb_ = std::move(cb); }
    void set_sig_quit_cb(SignalFunc&& cb) { sig_quit_cb_ = std::move(cb); }
    void set_sig_usr1_cb(SignalFunc&& cb) { sig_usr1_cb_ = std::move(cb); }
    void set_sig_usr2_cb(SignalFunc&& cb) { sig_usr2_cb_ = std::move(cb); }

    //开始loop前后处理函数
    void set_loop_befor_function(BeforLoopFunc&& cb) { loop_befor_func_ = std::move(cb); }
    void set_loop_after_function(AfterLoopFunc&& cb) { loop_after_func_ = std::move(cb); }

    void AssertInLoopThread() const
    {
        if(!IsInLoopThread())
        {
            AbortNotInLoopThread();
        }
    }

    bool IsInLoopThread() const { return tid_ == base::CurrentThread::tid(); }

    //如果调用线程是本EventLoop线程,RunInLoop会立刻执行,否则调用QueueInLoop排队,线程安全方法
    void RunInLoop(Task&& task);
    //排队调用任务
    void QueueInLoop(Task&& task);

    //定时任务
    TimerId TimerAt(base::Timestamp when, TimerCallback&& cb);
    TimerId TimerAfter(int delayS, TimerCallback&& cb);
    TimerId TimerInterval(int intervalS, TimerCallback&& cb);
    void TimerCancel(const TimerId& timer_id);

    //处理信号，只能在主线程调用
    void SetHandleSingnal();

public:
    static EventLoop* GetEventLoopOfCurrentThread();

    //设置日志参数
    static void SetLogger(const std::string& path, base::Logger::Level level,
            base::Logger::Level flush_level);

private:
    void AbortNotInLoopThread() const;

    //当poll没有外在事件发生时,poll阻塞返回需要最长等待时常,QueueInLoop和RunInLoop
    //也因此需要等待,为避免发生这样的情况,使用额外的事件来触发poll
    void Wakeup();
    void HandleWakeup();

    //处理信号
    void HandleSignal();

    //处理RunInLoop和QueueInLoop的请求
    void DoPendingTasks();


private:
    //改变Channel监听状态,由connection通过Channel发起改变请求,
    //Channel再通过EventLoop向poller请求改变
    friend class Channel;
    void UpdateChannel(Channel* channel);
    void RemoveChannel(Channel* channel);
    bool HasChannel(Channel* channel) const;

private:
    volatile bool looping_;
    volatile bool quit_;
    int tid_;
    const char* tname_;
    int64_t iteration_;

    //wakeup
    int wakeupfd_;
    std::shared_ptr<Channel> wakeup_channel_;

    //Task队列
    std::list<Task> task_list_; //guard by mutex_
    std::mutex mutex_;
    std::atomic<bool> need_wakup_;

    std::shared_ptr<Poller> poller_;
    std::shared_ptr<TimerQueue> timer_queue_;

    //signal 处理
    int sig_fd_;
    SignalFunc  sig_int_cb_;
    SignalFunc  sig_quit_cb_;
    SignalFunc  sig_usr1_cb_;
    SignalFunc  sig_usr2_cb_;
    std::shared_ptr<Channel> sig_channel_;

    //befor after run
    BeforLoopFunc loop_befor_func_;
    AfterLoopFunc loop_after_func_;
};


}//namespace net
//---------------------------------------------------------------------------
#endif //NET_EVENT_LOOP_H_
