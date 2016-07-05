//---------------------------------------------------------------------------
#include "event_loop.h"
#include "net_log.h"
#include "channel.h"
#include "timer_task_queue.h"
#include <poll.h>
#include <sys/eventfd.h>
#include <signal.h>
#include <sys/signalfd.h>
//---------------------------------------------------------------------------
namespace net
{
//---------------------------------------------------------------------------
namespace
{
//---------------------------------------------------------------------------
const int           kPollTimeS              = 5;
__thread EventLoop* t_loop_in_current_thread= 0;
//---------------------------------------------------------------------------
void InitSignal()
{
    //信号屏蔽需要在其他线程创建之前初始化,这样其他线程可以继承该屏蔽字,
    //因为Log里面带有线程,所以log要后初始化
 
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
        SystemLog_Error("BOLCK SIGPIPE failed");
        abort();
    }

    return;
}
//---------------------------------------------------------------------------
void InitLog()
{
    //init log
    MyNetLog = new NetLog();
    if(false == MyNetLog->Initialize("net io frame", "/tmp", "network_log", base::UNIT_MB))
    {
        SystemLog_Error("log initialze failed");
        abort();
    }
}
//---------------------------------------------------------------------------
void UninitLog()
{
    if(0 != MyNetLog)
    {
        MyNetLog->Uninitialize();
        delete MyNetLog;
        MyNetLog = 0;
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
        InitLog();
    }

    ~GlobalInit()
    {
        UninitLog();
    }
}g_handle_signal;
//---------------------------------------------------------------------------
int CreateEventFd()
{
    int fd = ::eventfd(0, EFD_NONBLOCK|EFD_CLOEXEC);
    if(0 > fd)
    {
        SystemLog_Error("eventfd create failed, errno:%d, msg:%s", errno, strerror(errno));
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
    poller_(new Poller()),
    timer_task_queue_(new TimerTaskQueue(this)),
    sig_fd_(0),
    d_event_handling_(false),
    d_current_active_channel_(nullptr)
{
    SystemLog_Info("event loop create:%p, in thread tid:%d, tname:%s", this, tid_, tname_);

    if(t_loop_in_current_thread)
    {
        SystemLog_Error("another event loop in this thread, loop:%p, tid:%d, tname:%s", t_loop_in_current_thread, tid_, tname_);
        assert(0);
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
    SystemLog_Info("event loop exit:%p, in thread tid:%d, tname:%s", this, tid_, tname_);

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
void EventLoop::Loop()
{
    assert(((void)"already looping!", !looping_));
    AssertInLoopThread();

    SystemLog_Info("%p Event loop start", this);

    looping_ = true;
    while(looping_)
    {
        active_channel_list_.clear();
        base::Timestamp time = poller_->Poll(kPollTimeS, &active_channel_list_);
        ++iteration_;
        
    #ifdef _DEBUG
        PrintActiveChannels();
    #endif
        
        d_event_handling_ = true;

            for(auto iter : active_channel_list_)
            {
                //0指针结尾来标识活动Channel结束，省去额外的有效下标变量
                if(nullptr == iter)
                    break;

                d_current_active_channel_ = iter;
                iter->HandleEvent(time);
            }

        d_current_active_channel_   = nullptr;
        d_event_handling_           = false;

        DoPendingTasks();
    }

    SystemLog_Info("%p Event loop stop", this);
    return;
}
//---------------------------------------------------------------------------
void EventLoop::Quit()
{
    SystemLog_Info("%p Event loop quit", this);

    looping_ = false;
    
    //wakup
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
        SystemLog_Error("create signal fd failed");
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
void EventLoop::AssertInLoopThread()
{
    if(!IsInLoopThread())
        AbortNotInLoopThread();

    return;
}
//---------------------------------------------------------------------------
bool EventLoop::IsInLoopThread()
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
TimerTask::Ptr EventLoop::RunAt(const base::Timestamp when, const CallbackTimerTask& callback)
{
    return timer_task_queue_->TimerTaskAdd(callback, when, 0);
}
//---------------------------------------------------------------------------
TimerTask::Ptr EventLoop::RunAfter(int delayS, const CallbackTimerTask& callback)
{
    base::Timestamp when = base::Timestamp::Now().AddTime(delayS);
    return timer_task_queue_->TimerTaskAdd(callback, when, 0);
}
//---------------------------------------------------------------------------
TimerTask::Ptr EventLoop::RunInterval(int intervalS, const CallbackTimerTask& callback)
{
    return timer_task_queue_->TimerTaskAdd(callback, base::Timestamp::Now(), intervalS);
}
//---------------------------------------------------------------------------
void EventLoop::RunCancel(TimerTask::Ptr timer_task)
{
    timer_task_queue_->TimerTaskCancel(timer_task);
}
//---------------------------------------------------------------------------
EventLoop* EventLoop::GetEventLoopOfCurrentThread()
{
    return t_loop_in_current_thread;
}
//---------------------------------------------------------------------------
void EventLoop::ChannelAdd(Channel* channel)
{
    assert(((void)"no in owner event loop", this == channel->owner_loop()));
    AssertInLoopThread();

    poller_->ChannelAdd(channel);
    return;
}
//---------------------------------------------------------------------------
void EventLoop::ChannelMod(Channel* channel)
{
    assert(((void)"no in owner event loop", this == channel->owner_loop()));
    AssertInLoopThread();

    poller_->ChannelMod(channel);
    return;
}
//---------------------------------------------------------------------------
void EventLoop::ChannelDel(Channel* channel)
{
    assert(((void)"no in owner event loop", this == channel->owner_loop()));
    AssertInLoopThread();

    poller_->ChannelDel(channel);
    return;
}
//---------------------------------------------------------------------------
void EventLoop::AbortNotInLoopThread()
{
    SystemLog_Debug("%p was create in tid:%u, tname:%s, but current tid:%d, tname:%s",
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
        SystemLog_Error("write failed");
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
        SystemLog_Error("read failed");
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

            SystemLog_Error("read failed, errno:%d, msg:%s", errno, strerror(errno));
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
            SystemLog_Error("recv error signal, signo:%d", siginfo.ssi_signo);
            assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
void EventLoop::DoPendingTasks()
{
    std::list<Task> task;
    need_wakup_ = true;

    //交换内容,避免处理的时候长时间锁住task_list_
    {
    std::lock_guard<std::mutex> lock(mutex_);
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
    for(auto iter : active_channel_list_)
    {
        if(nullptr == iter)
            break;

        std::cout << "{" << iter->REventsToString() << "}" << std::endl;
    }
}
//---------------------------------------------------------------------------
}//namespace net
