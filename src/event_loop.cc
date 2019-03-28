//---------------------------------------------------------------------------
#include <sys/eventfd.h>
#include <poll.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <unistd.h>
#include <cassert>
#include "event_loop.h"
#include "net_logger.h"
#include "poller.h"
#include "channel.h"
#include "timer_id.h"
#include "timer_queue.h"
//---------------------------------------------------------------------------
namespace net
{

namespace
{
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
        exit(-1);
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

            //设置默认的logger
            g_net_logger = base::Logger::stdout_logger_mt();
        }
}g_init;
//---------------------------------------------------------------------------
int CreateWakeupFd()
{
    int fd = ::eventfd(0, EFD_NONBLOCK|EFD_CLOEXEC);
    if(0 > fd)
    {
        NetLogger_error("eventfd create failed, errno:%d, msg:%s", errno, OSError(errno));
        exit(errno);
    }

    return fd;
}
//---------------------------------------------------------------------------
}//namespace

//---------------------------------------------------------------------------
__thread EventLoop* t_loop_in_current_thread = 0;
//---------------------------------------------------------------------------
EventLoop::EventLoop()
:   looping_(false),
    quit_(false),
    tid_(base::CurrentThread::tid()),
    tname_(base::CurrentThread::tname()),
    iteration_(0),
    wakeupfd_(CreateWakeupFd()),
    wakeup_channel_(new Channel(this, wakeupfd_, "wakeup")),
    poller_(Poller::NewDefaultPoller(this)),
    timer_queue_(new TimerQueue(this)),
    sig_fd_(0)
{
    NetLogger_trace("EventLoop create %p, in thread: %d, name:%s", this, tid_, tname_);

    if(t_loop_in_current_thread)
    {
        NetLogger_off("Another EventLoop %p exists in this thread", t_loop_in_current_thread);
        assert(0);
    }
    else
    {
        t_loop_in_current_thread = this;
    }

    wakeup_channel_->set_read_cb(std::bind(&EventLoop::HandleWakeup, this));
    wakeup_channel_->EnableReading();

    return;
}
//---------------------------------------------------------------------------
EventLoop::~EventLoop()
{
    NetLogger_trace("EventLoop destroy %p, in thread: %d, name:%s", this, tid_, tname_);

    assert(!looping_);
    t_loop_in_current_thread = 0;

    wakeup_channel_->DisableAll();
    wakeup_channel_->Remove();
    ::close(wakeupfd_);

    if(sig_channel_)
    {
        sig_channel_->DisableAll();
        sig_channel_->Remove();
        ::close(sig_fd_);
    }

    return;
}
//---------------------------------------------------------------------------
void EventLoop::Loop()
{
    assert(((void)"already looping!", !looping_));
    AssertInLoopThread();

    NetLogger_info("EventLoop(%p) loop start", this);

    looping_ = true;
    quit_ = false;

    while(!quit_)
    {
        uint64_t rcv_time = poller_->Poll(5);
        const std::vector<Channel*>& active_channels = poller_->active_channels();
        iteration_++;

        for(auto channel : active_channels)
        {
            if(nullptr == channel)
                break;

            channel->HandleEvent(rcv_time);
        }

        DoPendingTasks();

        //刷新日志
        g_net_logger->Flush();
    }

    //处理剩下的工作
    DoPendingTasks();

    NetLogger_info("EventLoop %p stop looping", this);
    looping_ = false;

    return;
}
//---------------------------------------------------------------------------
void EventLoop::Quit()
{
    quit_ = true;

    //wakup
    if(!IsInLoopThread())
    {
        Wakeup();
    }

    NetLogger_info("EventLoop(%p) loop quit", this);
    return;
}
//---------------------------------------------------------------------------
void EventLoop::RunInLoop(Task&& task)
{
    if(IsInLoopThread())
    {
        task();
    }
    else
    {
        QueueInLoop(std::move(task));
    }

    return;
}
//---------------------------------------------------------------------------
void EventLoop::QueueInLoop(Task&& task)
{
    {
        std::lock_guard<std::mutex> guard(mutex_);
        task_list_.push_back(std::move(task));
    }

    //如果不是在当前线程里面，目标线程有可能没有事件在处理,需要唤醒;又或者，即使
    //在当前线程，添加Task的在HandleEvent事件里面,或者在DoPendingTasks里面,前者不需
    //要唤醒，后者需要
    if(!IsInLoopThread() || need_wakup_)
    {
        Wakeup();
    }

    return;
}
//---------------------------------------------------------------------------
TimerId EventLoop::TimerAt(base::Timestamp when, TimerCallback&& cb)
{
    return timer_queue_->AddTimer(std::move(cb), when, 0);
}
//---------------------------------------------------------------------------
TimerId EventLoop::TimerAfter(int delayS, TimerCallback&& cb)
{
    base::Timestamp when = base::Timestamp::Now().AddTime(delayS);
    return timer_queue_->AddTimer(std::move(cb), when, 0);
}
//---------------------------------------------------------------------------
TimerId EventLoop::TimerInterval(int intervalS, TimerCallback&& cb)
{
    return timer_queue_->AddTimer(std::move(cb), base::Timestamp::Now(), intervalS);
}
//---------------------------------------------------------------------------
void EventLoop::TimerCancel(const TimerId& timer_id)
{
    timer_queue_->Cancel(timer_id);
}
//---------------------------------------------------------------------------
void EventLoop::SetHandleSingnal()
{
    assert(((void)"can only handle signal in main thread", base::CurrentThread::IsMainThread()));
    assert(((void)"already have signal channel", sig_channel_.get()==0));

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
        exit(-1);
    }
    sig_fd_ = sfd;

    sig_channel_.reset(new Channel(this, sig_fd_));
    sig_channel_->set_read_cb(std::bind(&EventLoop::HandleSignal, this));
    sig_channel_->EnableReading();

    return;
}
//---------------------------------------------------------------------------
 EventLoop* EventLoop::GetEventLoopOfCurrentThread()
{
    return t_loop_in_current_thread;
}
//---------------------------------------------------------------------------
void EventLoop::SetLogger(const std::string& path, base::Logger::Level level,
        base::Logger::Level flush_level)
{
    if(!path.empty())
    {
        g_net_logger = base::Logger::file_stdout_logger_mt(path);
    }

    g_net_logger->set_level(level);
    g_net_logger->set_flush_level(flush_level);

    return;
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
    }

    return;
}
//---------------------------------------------------------------------------
void EventLoop::HandleSignal()
{
    struct signalfd_siginfo siginfo;
    ssize_t len = sizeof(siginfo);
    ssize_t offset = 0;
    while(len)
    {
        ssize_t rlen = read(sig_fd_, reinterpret_cast<char*>(&siginfo)+offset, len);
        if(-1 == rlen)
        {
            if(EINTR==errno || (EAGAIN==errno))
                continue;

            NetLogger_error("read signal failed, errno:%d, msg:%s", errno, OSError(errno));
                return;
        }

        len -= rlen;
        offset += rlen;
    }

    switch(siginfo.ssi_signo)
    {
        case SIGINT:
            if(sig_int_cb_)
                sig_int_cb_();

            break;

        case SIGQUIT:
            if(sig_quit_cb_)
                sig_quit_cb_();

            break;

        case SIGUSR1:
            if(sig_usr1_cb_)
                sig_usr1_cb_();

            break;

        case SIGUSR2:
            if(sig_usr2_cb_)
                sig_usr2_cb_();

            break;

        default:
            NetLogger_error("recv error signal, signo:%d", siginfo.ssi_signo);
            assert(((void)"recv error signal", 0));
    }

    return;
}
//---------------------------------------------------------------------------
void EventLoop::DoPendingTasks()
{
    std::list<Task> tasks;
    need_wakup_ = true;

    //交换，避免长时间锁住
    {
        std::lock_guard<std::mutex> guard(mutex_);
        tasks.swap(task_list_);
    }

    for(auto task : tasks)
    {
        task();
    }

    need_wakup_ = false;
    return;
}
//---------------------------------------------------------------------------
void EventLoop::UpdateChannel(Channel* channel)
{
    AssertInLoopThread();
    assert(((void)"channel not in current eventloop", this == channel->OwnerLoop()));

    poller_->UpdateChannel(channel);
    return;
}
//---------------------------------------------------------------------------
void EventLoop::RemoveChannel(Channel* channel)
{
    AssertInLoopThread();
    assert(((void)"channel not in current eventloop", this == channel->OwnerLoop()));

    poller_->RemoveChannel(channel);
    return;
}
//---------------------------------------------------------------------------
bool EventLoop::HasChannel(Channel* channel) const
{
    AssertInLoopThread();
    assert(((void)"channel not in current eventloop", this == channel->OwnerLoop()));

    return poller_->HasChannel(channel);
}
//---------------------------------------------------------------------------

}//namespace net
//---------------------------------------------------------------------------


