//---------------------------------------------------------------------------
#ifndef NET_TIMER_TASK_ID_H_
#define NET_TIMER_TASK_ID_H_
#include <stdint.h>
//---------------------------------------------------------------------------
namespace net
{

class TimerTask;

//---------------------------------------------------------------------------
class TimerTaskId
{
public:
    TimerTaskId(TimerTask* timer_task, int64_t id)
    :   timer_task_(timer_task),
        id_(id)
    {
    }
    ~TimerTaskId()
    {
    }

    friend class TimerTaskQueue;

private:
    TimerTask*  timer_task_;
    int64_t     id_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //NET_TIMER_TASK_ID_H_
