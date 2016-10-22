//---------------------------------------------------------------------------
#include <unistd.h>
#include <sys/timerfd.h>
#include <sys/timerfd.h>
#include <cstring>
#include <cassert>
#include "timer_task.h"
#include "timer_task_queue.h"
#include "event_loop.h"
#include "channel.h"
#include "net_log.h"
//---------------------------------------------------------------------------
namespace net
{

static int CreateTimerfd()
{
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK|TFD_CLOEXEC);
    if(0 > timerfd)
    {
        SystemLog_Error("timerfd create failed, errno:%d, msg:%s", errno, StrError(errno));
        abort();
    }

    return timerfd;
}
//---------------------------------------------------------------------------
static struct timespec HowManyTimeLeftNow(base::Timestamp when)
{
    int64_t micro_seconds = when - base::Timestamp::Now();
    if(100 > micro_seconds)
        micro_seconds = 100;

    struct timespec ts;
    ts.tv_sec   = micro_seconds / base::Timestamp::kMicrosecondsPerSecond;
    ts.tv_nsec  = micro_seconds % base::Timestamp::kMicrosecondsPerSecond * 1000;
    return ts;
}
//---------------------------------------------------------------------------
static void ReadTimer(int timerfd)
{
    uint64_t dat;
    ssize_t rlen = ::read(timerfd, &dat, sizeof(dat));
    if(rlen != sizeof(dat))
    {
        SystemLog_Error("ReadTimer failed");
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
static void ResetTimerfd(int timerfd, base::Timestamp expired)
{
    struct itimerspec new_val;
    struct itimerspec old_val;
    bzero(&new_val, sizeof(itimerspec));
    bzero(&old_val, sizeof(itimerspec));
    new_val.it_value = HowManyTimeLeftNow(expired);
    int err_code = ::timerfd_settime(timerfd, 0, &new_val, &old_val);
    if(0 > err_code)
    {
        SystemLog_Error("timerfd_settime error.errno:%d, msg:%s", errno, StrError(errno));
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
TimerTaskQueue::TimerTaskQueue(EventLoop* owner_loop)
:   owner_loop_(owner_loop)
{
    SystemLog_Debug("timer task queue conn_ptr");

    timerfd_ = CreateTimerfd();
    channel_timer_.reset(new Channel(owner_loop_, timerfd_));

    channel_timer_->set_callback_read(std::bind(&TimerTaskQueue::HandRead, this));
    channel_timer_->ReadEnable();

    return;
}
//---------------------------------------------------------------------------
TimerTaskQueue::~TimerTaskQueue()
{
    SystemLog_Debug("timer task queue destory");

    channel_timer_->DisableAll();
    channel_timer_->Remove();

    ::close(timerfd_);

    for(auto iter : entry_list_)
        delete iter.second;

    return;
}
//---------------------------------------------------------------------------
TimerTaskId TimerTaskQueue::TimerTaskAdd(TimerTask::TimerTaskCallback&& callback, base::Timestamp when, int intervalS)
{
    TimerTask* timer_task = new TimerTask(std::move(callback), when, intervalS);
    owner_loop_->RunInLoop(std::bind(&TimerTaskQueue::AddTimerInLoop, this, timer_task));

    return TimerTaskId(timer_task, timer_task->id());
}
//---------------------------------------------------------------------------
void TimerTaskQueue::TimerTaskCancel(TimerTaskId timer_task_id)
{
    owner_loop_->RunInLoop(std::bind(&TimerTaskQueue::CancelTimerInLoop, this, timer_task_id));

    return;
}
//---------------------------------------------------------------------------
void TimerTaskQueue::AddTimerInLoop(TimerTask* timer_task)
{
    owner_loop_->AssertInLoopThread();

    bool earliest = Insert(const_cast<TimerTask*>(timer_task));
    if(true == earliest)
        ResetTimerfd(timerfd_, timer_task->expairation());

    return;
}
//---------------------------------------------------------------------------
void TimerTaskQueue::CancelTimerInLoop(TimerTaskId timer_task_id)
{
    owner_loop_->AssertInLoopThread();

    if(0 == timer_task_id.timer_task_)
        return;

    auto iter = entry_list_.find(Entry(timer_task_id.timer_task_->expairation(), timer_task_id.timer_task_));
    if(entry_list_.end() == iter)
        return;

    entry_list_.erase(iter);
    delete iter->second;
    return;
}
//---------------------------------------------------------------------------
void TimerTaskQueue::HandRead()
{
    owner_loop_->AssertInLoopThread();

    base::Timestamp now = base::Timestamp::Now();
    ReadTimer(timerfd_);

    std::vector<Entry> expired = GetExpired(now);
    for(auto iter : expired)
        iter.second->Run();

    Reset(expired);
}
//---------------------------------------------------------------------------
std::vector<TimerTaskQueue::Entry> TimerTaskQueue::GetExpired(base::Timestamp now)
{
    std::vector<Entry>  expired;
    Entry               sentry(now, reinterpret_cast<TimerTask*>(UINTPTR_MAX));
    auto                end = entry_list_.lower_bound(sentry);

    std::copy(entry_list_.begin(), end, back_inserter(expired));
    entry_list_.erase(entry_list_.begin(), end);

    return expired;
}
//---------------------------------------------------------------------------
void TimerTaskQueue::Reset(std::vector<Entry>& expired)
{
    for(auto iter : expired)
    {
        if(0 < iter.second->interval())
        {
            iter.second->Restart();
            Insert(iter.second);
        }
        else
            delete iter.second;
    }

    if(!entry_list_.empty())
    {
        base::Timestamp next_expired = entry_list_.begin()->second->expairation();
        ResetTimerfd(timerfd_, next_expired);
    }

    return;
}
//---------------------------------------------------------------------------
bool TimerTaskQueue::Insert(TimerTask* timer_task)
{
    bool earliest = false;
   
    base::Timestamp when = timer_task->expairation();
    auto            iter = entry_list_.begin();
    if((iter==entry_list_.end()) || (when<iter->first))
        earliest = true;

    if(false == entry_list_.insert(Entry(when, timer_task)).second)
    {
        SystemLog_Error("insert timer_task error");
        assert(0);
        return false;
    }

    return earliest;
}
//---------------------------------------------------------------------------
}//namespace net
