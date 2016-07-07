//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_TIMER_TASK_H_
#define BASE_LINUX_NET_TIMER_TASK_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
#include "../base/timestamp.h"
#include <atomic>
//---------------------------------------------------------------------------
namespace net
{

class TimerTask
{
public:
    typedef std::function<void (void)>  TimerTaskCallback;

    TimerTask(TimerTaskCallback&& callback, base::Timestamp when, int intervalS=0)
    :   task_callback_(std::move(callback)),
        expairation_(when),
        interval_(intervalS),
        id_(no_++)
    {
    }
    ~TimerTask()
    {
    }

    void Run() { task_callback_();} //运行定时任务

    void Restart(); //如果任务是间隔时间运行的,则重启任务

    base::Timestamp expairation() const { return expairation_; }
    bool            interval() const    { return interval_; }
    int64_t         id() const          { return id_; }

private:
    const TimerTaskCallback task_callback_;
    base::Timestamp         expairation_;
    int                     interval_;
    const int64_t           id_;

protected:
    DISALLOW_COPY_AND_ASSIGN(TimerTask);

private:
    static std::atomic<int64_t> no_;
};

}//namespace net
#endif// BASE_LINUX_NET_TIMER_TASK_H_
