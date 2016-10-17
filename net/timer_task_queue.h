//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_TIMER_TASK_QUEUE_H_
#define BASE_LINUX_NET_TIMER_TASK_QUEUE_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
#include "timer_task_id.h"
#include "timer_task.h"
//---------------------------------------------------------------------------
namespace net
{

class EventLoop;
class Channel;
class TimerTask;

class TimerTaskQueue
{
public:
    TimerTaskQueue(EventLoop* owner_loop);
    ~TimerTaskQueue();

    //add timer task
    TimerTaskId TimerTaskAdd(TimerTask::TimerTaskCallback&& callback, base::Timestamp when, int intervalS);

    //cancel timer task, can't cannel itself!!!
    void TimerTaskCancel(TimerTaskId timer_task_id);

    size_t TimerTaskCount() const { return entry_list_.size(); }

private:
    typedef std::pair<base::Timestamp, TimerTask*>  Entry;
    typedef std::set<Entry>                         EntryList;

    void AddTimerInLoop     (TimerTask* timer_task);
    void CancelTimerInLoop  (TimerTaskId timer_task_id);

    void HandRead();

    std::vector<Entry>  GetExpired  (base::Timestamp now);
    void                Reset       (std::vector<Entry>& expired);

    bool Insert(TimerTask* timer_task);

private:
    int                         timerfd_;
    EventLoop*                  owner_loop_;
    EntryList                   entry_list_;
    std::shared_ptr<Channel>    channel_timer_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //BASE_LINUX_NET_TIMER_TASK_QUEUE_H_
