
//---------------------------------------------------------------------------
#include "timer_task.h"
//---------------------------------------------------------------------------
namespace net
{

TimerTask::TimerTask(TimerTaskCallback& callback, base::Timestamp when, int intervalS)
:   task_callback_(std::move(callback)),
    expairation_(when),
    interval_(intervalS)
{
}
//---------------------------------------------------------------------------
TimerTask::~TimerTask()
{
}
//---------------------------------------------------------------------------
void TimerTask::Restart()
{
    if(0 < interval_)
        expairation_ = base::Timestamp::Now().AddTime(interval_);

    return ;
}

}//namespace net
//---------------------------------------------------------------------------
