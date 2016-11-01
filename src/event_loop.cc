//---------------------------------------------------------------------------
#include <sys/eventfd.h>
#include <poll.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <cassert>
#include <unistd.h>
#include "event_loop.h"
#include "net_log.h"
#include "channel.h"
#include "timer_task_queue.h"
#include "poller.h"
#include "../depend/base/include/thread.h"
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
namespace
{

//---------------------------------------------------------------------------
const int kPollTimeS = 5;
__thread EventLoop* t_loop_in_current_thread = 0;
//---------------------------------------------------------------------------
void InitSignal()
{
    //信号屏蔽需要在其他线程创建之前初始化,这样其他线程可以继承该屏蔽字,
 
    //block signal
    sigset_t signal_mask;
    sigemptyset(&signal_mask);
    sigaddset(&signal_mask, SIGPIPE);
    sigaddset(&signal_mask, SIGINT);
    sigaddset(&signal_mask, SIGQUIT);
    sigaddset(&signal_mask, SIGUSR1);
    sigaddset(&signal_mask, SIGUSR2);

    if(-1 == pthread_sigmask(SIG_BLOCK, &signal_mask, NULL))
    {
        abort();
    }

    return;
}
//---------------------------------------------------------------------------
class GlobalInit 
{
public:
    GlobalInit()
    {
        InitSignal();
    }

    ~GlobalInit()
    {
    }
}g_init;
//---------------------------------------------------------------------------
int CreateEventFd()
{
    int fd = ::eventfd(0, EFD_NONBLOCK|EFD_CLOEXEC);
    if(0 > fd)
    {
        NetLogger_off("eventfd create failed, errno:%d, msg:%s", errno, OSError(errno));
        abort();
    }

    return fd;
}
}//namespace
//---------------------------------------------------------------------------
EventLoop::EventLoop()
:   looping_(false),
    tid_(base::CurrentThread::tid()),
    tname_(base::CurrentThread::tname()),
    iteration_(0),
    wakeupfd_(CreateEventFd()),
    channel_wakeup_(new Channel(this, wakeupfd_)),
    need_wakup_(true),
    poller_(Poller::NewDefaultPoller(this)),
    timer_task_queue_(new TimerTaskQueue(this)),
    sig_fd_(0)
{
    NetLogger_info("event loop create:%p, in thread tid:%d, tname:%s", this, tid_, tname_);

    if(t_loop_in_current_thread)
    {
        NetLogger_off("another event loop in this thread, loop:%p, tid:%d, tname:%s", t_loop_in_current_thread, tid_, tname_);
        abort();
        return;
    }
    
    channel_wakeup_->set_callback_read(std::bind(&EventLoop::HandleWakeup, this));
    channel_wakeup_->ReadEnable();

    t_loop_in_current_thread = this;
    return;
}
//---------------------------------------------------------------------------
EventLoop::~EventLoop()
{
    NetLogger_info("event loop exit:%p, in thread tid:%d, tname:%s", this, tid_, tname_);

    channel_wakeup_->DisableAll();
    channel_wakeup_->Remove();
    ::close(wakeupfd_);

    if(channel_sig_)
    {
        channel_sig_->DisableAll();
        channel_sig_->Remove();
        ::close(sig_fd_);
    }

    t_loop_in_current_thread = 0;
    return;
}
//---------------------------------------------------------------------------
void EventLoop::SetLogger(const std::string& path, int lv)
{
    logger = base::Logger::file_stdout_logger_mt("debug", path, "net", "log", true);

    logger->set_level(base::Logger::Level(lv));
    logger->set_flush_level(base::Logger::Level::ERROR);

    return;
}
//---------------------------------------------------------------------------
void EventLoop::Loop()
{
    assert(((void)"already looping!", !looping_));
    AssertInLoopThread();

    NetLogger_info("%p Event loop start", this);

    looping_ = true;
    while(looping_)
    {
        if(loop_befor_func_) loop_befor_func_();

        uint64_t time = poller_->Poll(kPollTimeS);
        const std::vector<Channel*>& active_channel_list = poller_->active_channels();
        ++iteration_;
        
        for(auto iter : active_channel_list)
        {
            if(nullptr == iter)
                break;

            iter->HandleEvent(time);
        }

        DoPendingTasks();

        if(loop_after_func_) loop_after_func_();
        logger->Flush(); //刷新日志
    }

    //处理完成工作
    DoPendingTasks();

    NetLogger_info("%p Event loop stop", this);
    return;
}
//---------------------------------------------------------------------------
void EventLoop::Quit()
{
    NetLogger_info("%p Event loop quit", this);

    looping_ = false;
    
    //wakup
    if(!IsInLoopThread())
        Wakeup();

    return;
}
//---------------------------------------------------------------------------
void EventLoop::SetAsSignalHandleEventLoop()
{
    assert(((void)"can only handle signal in main thread", base::CurrentThread::IsMainThread()));

    //handel sig through epoll
    sigset_t signal_mask;
    sigemptyset(&signal_mask);
    sigaddset(&signal_mask, SIGINT);
    sigaddset(&signal_mask, SIGQUIT);
    sigaddset(&signal_mask, SIGUSR1);
    sigaddset(&signal_mask, SIGUSR2);
    int sfd = signalfd(-1, &signal_mask, SFD_NONBLOCK|SFD_CLOEXEC);
    if(-1 == sfd)
    {
        NetLogger_off("create signal fd failed");
        abort();
        return;
    }
    sig_fd_ = sfd;

    channel_sig_.reset(new Channel(this, sig_fd_));
    channel_sig_->set_callback_read(std::bind(&EventLoop::HandleSignal, this));
    channel_sig_->ReadEnable();

    return;
}
//---------------------------------------------------------------------------
void EventLoop::AssertInLoopThread() const
{
    if(!IsInLoopThread())
        AbortNotInLoopThread();

    return;
}
//---------------------------------------------------------------------------
bool EventLoop::IsInLoopThread() const
{
    return (tid_ == base::CurrentThread::tid());

}
//---------------------------------------------------------------------------
void EventLoop::RunInLoop(Task&& task)
{
    //为了线程安全设计,所有的回调函数子都会在Eventloop中顺序处理,不会有线程安全问题
    //
    //参数都是复制传递的.
    
    if(true == IsInLoopThread())
    {
        task();
        return;
    }

    //添加到队列中排队处理
    QueueInLoop(std::move(task));
    return;
}
//---------------------------------------------------------------------------
void EventLoop::QueueInLoop(Task&& task)
{
    {
    std::lock_guard<std::mutex> lock(mutex_);
    task_list_.push_back(task);
    }

    //如果不是在EventLoop线程内调用
    //或者EventLoop线程在不在处理事件中,则需要唤醒loop(fix:好像还是有点问题,因为在处理过程中is_pending_task变量值有窗口)
    if((!IsInLoopThread()) || need_wakup_)
    {
        Wakeup();
        need_wakup_ = false;
    }

    return;
}
//---------------------------------------------------------------------------
TimerTaskId EventLoop::RunAt(uint64_t when, CallbackTimerTask&& callback)
{
    return timer_task_queue_->TimerTaskAdd(std::move(callback), when, 0);
}
//---------------------------------------------------------------------------
TimerTaskId EventLoop::RunAfter(int delayS, CallbackTimerTask&& callback)
{
    uint64_t when  = base::Timestamp::Now().AddTime(delayS).Microseconds();
    return timer_task_queue_->TimerTaskAdd(std::move(callback), when, 0);
}
//---------------------------------------------------------------------------
TimerTaskId EventLoop::RunInterval(int intervalS, CallbackTimerTask&& callback)
{
    return timer_task_queue_->TimerTaskAdd(std::move(callback), base::Timestamp::Now().Microseconds(), intervalS);
}
//---------------------------------------------------------------------------
void EventLoop::RunCancel(TimerTaskId timer_task_id)
{
    timer_task_queue_->TimerTaskCancel(timer_task_id);
}
//---------------------------------------------------------------------------
EventLoop* EventLoop::GetEventLoopOfCurrentThread()
{
    return t_loop_in_current_thread;
}
//---------------------------------------------------------------------------
void EventLoop::ChannelUpdate(Channel* channel)
{
    assert(this == channel->owner_loop());
    AssertInLoopThread();

    poller_->ChannelUpdate(channel);
    return;
}
//---------------------------------------------------------------------------
void EventLoop::ChannelRemove(Channel* channel)
{
    assert(this == channel->owner_loop());
    AssertInLoopThread();

    poller_->ChannelRemove(channel);
    return;
}
//---------------------------------------------------------------------------
bool EventLoop::HasChannel(Channel* channel) const
{
    assert(this == channel->owner_loop());
    AssertInLoopThread();

    return poller_->HasChannel(channel);
}
//---------------------------------------------------------------------------
void EventLoop::AbortNotInLoopThread() const
{
    NetLogger_off("%p was create in tid:%u, tname:%s, but current tid:%d, tname:%s",
                    this, tid_, tname_, base::CurrentThread::tid(), base::CurrentThread::tname());

    assert(0);
    return;
}
//---------------------------------------------------------------------------
void EventLoop::Wakeup()
{
    eventfd_t dat = 1;
    if(-1 == eventfd_write(wakeupfd_, dat))
    {
        NetLogger_error("write failed, errno:%d, msg:%s", errno, OSError(errno));
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
void EventLoop::HandleWakeup()
{
    eventfd_t dat;
    if(-1 == eventfd_read(wakeupfd_, &dat))
    {
        NetLogger_error("read failed, errno:%d, msg:%s", errno, OSError(errno));
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
void EventLoop::HandleSignal()
{
    struct signalfd_siginfo siginfo;
    ssize_t len     = sizeof(siginfo);
    ssize_t offset  = 0;
    while(len)
    {
        ssize_t rlen = read(sig_fd_, reinterpret_cast<char*>(&siginfo)+offset, len);
        if(-1 == rlen)
        {
            if(EINTR==errno || (EAGAIN==errno))
                continue;

            NetLogger_error("read failed, errno:%d, msg:%s", errno, OSError(errno));
            return;
        }

        len     -= rlen;
        offset  += rlen;
    }

    switch(siginfo.ssi_signo)
    {
        case SIGINT:
            if(sig_int_callback_)
                sig_int_callback_();

            break;

        case SIGQUIT:
            if(sig_quit_callback_)
                sig_quit_callback_();

            break;

        case SIGUSR1:
            if(sig_usr1_callback_)
                sig_usr1_callback_();

            break;

        case SIGUSR2:
            if(sig_usr2_callback_)
                sig_usr2_callback_();

            break;

        default:
            NetLogger_error("recv error signal, signo:%d", siginfo.ssi_signo);
            assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
void EventLoop::DoPendingTasks()
{
    std::list<Task> task;

    //交换内容,避免处理的时候长时间锁住task_list_
    {
    std::lock_guard<std::mutex> lock(mutex_);

    need_wakup_ = true;
    task.swap(task_list_);
    }

    for(auto iter : task)
    {
        iter();
    }

    return;
}
//---------------------------------------------------------------------------
void EventLoop::PrintActiveChannels() const
{
    for(auto iter : poller_->active_channels())
    {
        if(nullptr == iter)
            break;

        NetLogger_trace("{ fd:%d event:%s }", iter->fd(), iter->REventsToString().c_str());
    }

    return;
}
//---------------------------------------------------------------------------

}//namespace net
