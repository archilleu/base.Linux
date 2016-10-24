
//---------------------------------------------------------------------------
#include "timer_task.h"
#include "../depend/base/include/timestamp.h"
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
std::atomic<int64_t> TimerTask::no_ = ATOMIC_VAR_INIT(0);
//---------------------------------------------------------------------------
void TimerTask::Restart()
{
    if(0 < interval_)
        expairation_ = base::Timestamp::Now().AddTime(interval_).Microseconds();

    return ;
}
//---------------------------------------------------------------------------

}//namespace net
