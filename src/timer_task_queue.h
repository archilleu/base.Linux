//---------------------------------------------------------------------------
#ifndef NET_TIMER_TASK_QUEUE_H_
#define NET_TIMER_TASK_QUEUE_H_
//---------------------------------------------------------------------------
#include <set>
#include <vector>
#include <memory>
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
    TimerTaskId TimerTaskAdd(TimerTask::TimerTaskCallback&& callback, uint64_t when, int intervalS);

    //cancel timer task, can't cannel itself!!!
    void TimerTaskCancel(TimerTaskId timer_task_id);

    size_t TimerTaskCount() const { return entry_list_.size(); }

private:
    using Entry = std::pair<uint64_t, TimerTask*> ;
    using EntryList = std::set<Entry>;

    void AddTimerInLoop     (TimerTask* timer_task);
    void CancelTimerInLoop  (TimerTaskId timer_task_id);

    void HandRead();

    std::vector<Entry>  GetExpired  (uint64_t now);
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
#endif //NET_TIMER_TASK_QUEUE_H_
