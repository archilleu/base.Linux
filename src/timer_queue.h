//---------------------------------------------------------------------------
#ifndef NET_TIMER_QUEUE_H_ 
#define NET_TIMER_QUEUE_H_ 
//---------------------------------------------------------------------------
#include <set>
#include <vector>
#include <memory>
#include <atomic>
#include "channel.h"
#include "../thirdpart/base/include/timestamp.h"
#include "callback.h"
//---------------------------------------------------------------------------
namespace net
{

class Timer;
class EventLoop;
class TimerId;

class TimerQueue
{
public:
    TimerQueue(EventLoop* event_loop);
    ~TimerQueue();

public:
    TimerId AddTimer(TimerCallback&& cb, base::Timestamp when, int intervalS);
    void Cancel(const TimerId& timer_id);

private:
    using Entry = std::pair<base::Timestamp, Timer*>;
    using TimerList = std::set<Entry>;

    //外部保存的是TimerId，所以需要下面的结构记录追踪TimerId用来取消Timer
    //Entry没办法作为区分TimerId的，因为Timestamp是变动的（interval timer）
    using ActiveTimer = std::pair<Timer*, uint64_t>;
    using ActiveTimerSet = std::set<ActiveTimer>;

private:
    void AddTimerInLoop(Timer* timer);
    void CancelInLoop(const TimerId& timer_id);

    void HandleRead();

    std::vector<Entry> getExpired(base::Timestamp now);
    void Reset(const std::vector<Entry>& expired, base::Timestamp now);
    bool Insert(Timer* timer);

private:

    EventLoop* event_loop_;
    const int timerfd_;
    Channel timerfd_channel_;

    TimerList timers_;

    //for Ccancle
    ActiveTimerSet active_timers_;
    std::atomic<bool> calling_expired_timers_;
    ActiveTimerSet canceling_timers_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //NET_TIMER_QUEUE_H_
