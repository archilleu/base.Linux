//---------------------------------------------------------------------------
#ifndef NET_TIMER_TASK_H_
#define NET_TIMER_TASK_H_
//---------------------------------------------------------------------------
#include <atomic>
#include <functional>
//---------------------------------------------------------------------------
namespace net
{

class TimerTask
{
public:
    using TimerTaskCallback = std::function<void (void)>;

    TimerTask(TimerTaskCallback&& callback, uint64_t when, int intervalS=0)
    :   task_callback_(std::move(callback)),
        expairation_(when),
        interval_(intervalS),
        id_(no_++)
    {
    }
    ~TimerTask()
    {
    }
    TimerTask(const TimerTask&) =delete;
    TimerTask& operator=(const TimerTask&) =delete;

    void Run() { task_callback_();} //运行定时任务

    void Restart(); //如果任务是间隔时间运行的,则重启任务

    uint64_t    expairation() const { return expairation_; }
    bool        interval() const    { return interval_; }
    int64_t     id() const          { return id_; }

private:
    const TimerTaskCallback task_callback_;
    uint64_t expairation_;
    int interval_;
    const int64_t id_;

private:
    static std::atomic<int64_t> no_;
};

}//namespace net
#endif// NET_TIMER_TASK_H_
