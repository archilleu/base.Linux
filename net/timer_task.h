//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_TIMER_TASK_H_
#define BASE_LINUX_NET_TIMER_TASK_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
#include "../base/timestamp.h"
//---------------------------------------------------------------------------
namespace net
{

class TimerTask
{
public:
    typedef std::shared_ptr<TimerTask>  Ptr;
    typedef std::function<void (void)>  TimerTaskCallback;

    TimerTask(const TimerTaskCallback& callback, base::Timestamp when, int intervalS=0);
    ~TimerTask();

    void Run() { task_callback_();} //运行定时任务

    void Restart(); //如果任务是间隔时间运行的,则重启任务

    base::Timestamp expairation()   { return expairation_; }
    bool            interval()      { return interval_; }

private:
    TimerTaskCallback   task_callback_;
    base::Timestamp     expairation_;
    int                 interval_;

protected:
//    DISALLOW_COPY_AND_ASSIGN(TimerTask);
};

}//namespace net
#endif// BASE_LINUX_NET_TIMER_TASK_H_
