//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_TIMER_TASK_QUEUE_H_
#define BASE_LINUX_NET_TIMER_TASK_QUEUE_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
#include "timer_task.h"
//---------------------------------------------------------------------------
namespace net
{

class EventLoop;
class Channel;

class TimerTaskQueue
{
public:
    TimerTaskQueue(EventLoop* owner_loop);
    ~TimerTaskQueue();

    TimerTask::Ptr  TimerTaskAdd    (const TimerTask::TimerTaskCallback& callback, base::Timestamp when, int intervalS);
    void            TimerTaskCancel (const TimerTask::Ptr timer_task);

private:
    typedef std::pair<base::Timestamp, TimerTask::Ptr>  Entry;
    typedef std::set<Entry>                             EntryList;

    void AddTimerInLoop     (const TimerTask::Ptr timer_task);
    void CancelTimerInLoop  (const TimerTask::Ptr timer_task);

    void HandRead();

    std::vector<Entry>  GetExpired  (base::Timestamp now);
    void                Reset       (const std::vector<Entry>& expired, base::Timestamp now);

    bool Insert(const TimerTask::Ptr timer_task);

private:
    int                         timerfd_;
    EventLoop*                  owner_loop_;
    EntryList                   entry_list_;
    std::shared_ptr<Channel>    channel_timer_;
};

}//namespace net
#endif //BASE_LINUX_NET_TIMER_TASK_QUEUE_H_
