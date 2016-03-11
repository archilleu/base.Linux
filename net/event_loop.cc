//---------------------------------------------------------------------------
#include "event_loop.h"
#include "net_log.h"
#include "channel.h"
#include <poll.h>
#include <sys/eventfd.h>
//---------------------------------------------------------------------------
namespace net
{
//---------------------------------------------------------------------------
static int CreateEventFd()
{
    int fd = ::eventfd(0, EFD_NONBLOCK|EFD_CLOEXEC);
    if(0 > fd)
    {
        char buffer[128];
        SystemLog_Error("eventfd create failed, errno:%d, msg:%s", errno, strerror_r(errno, buffer, 128));
    }

    return fd;
}
//---------------------------------------------------------------------------
EventLoop::EventLoop()
:   looping_(false),
    tid_(base::CurrentThread::Tid()),
    tname_(base::CurrentThread::ThreadName()),
    is_pending_task_(false),
    poller_(new Poller())
{
    wakeupfd_ = CreateEventFd();
    channel_wakeup_.reset(new Channel(this, wakeupfd_));

    channel_wakeup_->set_callback_read(std::bind(&EventLoop::HandleRead, this));
    channel_wakeup_->ReadEnable();

    SystemLog_Debug("event loop create:%p, in thread tid:%u, tname:%s", this, tid_, tname_);
    return;
}
//---------------------------------------------------------------------------
EventLoop::~EventLoop()
{
    assert(!looping_);
    SystemLog_Debug("event loop exit:%p, in thread tid:%u, tname:%s", this, tid_, tname_);
}
//---------------------------------------------------------------------------
void EventLoop::Loop()
{
    assert(!looping_);
    AssertInLoopThread();

    SystemLog_Info("%p Event loop start", this);

    looping_ = true;
    while(looping_)
    {
        base::Timestamp rcv_time = poller_->Poll(5, &active_channel_list_);
        for(auto iter=active_channel_list_.begin(); active_channel_list_.end()!=iter; ++iter)
        {
            (*iter)->HandleEvent(rcv_time);
        }
    }

    SystemLog_Info("%p Event loop stop", this);
    return;
}
//---------------------------------------------------------------------------
void EventLoop::Quit()
{
    looping_ = false;
    //todo wakup
}
//---------------------------------------------------------------------------
void EventLoop::AssertInLoopThread()
{
    if(!IsInLoopThread())
    {
        AbortNotInLoopThread();
    }

    return;
}
//---------------------------------------------------------------------------
bool EventLoop::IsInLoopThread()
{
    return (tid_ == base::CurrentThread::Tid());
}
//---------------------------------------------------------------------------
void EventLoop::RunInLoop(const Task& task)
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
    QueueInLoop(task);
    return;
}
//---------------------------------------------------------------------------
void EventLoop::QueueInLoop(const Task& task)
{
    {
    std::lock_guard<std::mutex> lock(mutex_);
    task_list_.push_back(task);
    }

    //如果不是在EventLoop线程内调用或者EventLoop线程不在处理实践中,则需要唤醒loop
    if((!IsInLoopThread()) || (!is_pending_task_))
    {
        Wakeup();
    }

    return;
}
//---------------------------------------------------------------------------
void EventLoop::ChannelAdd(Channel* channel)
{
    assert(this == channel->owner_loop());
    AssertInLoopThread();

    poller_->ChannelAdd(channel);
    return;
}
//---------------------------------------------------------------------------
void EventLoop::ChannelMod(Channel* channel)
{
    assert(this == channel->owner_loop());
    AssertInLoopThread();

    poller_->ChannelMod(channel);
    return;
}
//---------------------------------------------------------------------------
void EventLoop::ChannelDel(Channel* channel)
{
    assert(this == channel->owner_loop());
    AssertInLoopThread();

    poller_->ChannelDel(channel);
    return;
}
//---------------------------------------------------------------------------
void EventLoop::AbortNotInLoopThread()
{
    SystemLog_Debug("%p was create in tid:%u, tname:%s, but current tid:%u, tname:%s",
                    this, tid_, tname_, base::CurrentThread::Tid(), base::CurrentThread::ThreadName());

    assert(0);
    return;
}
//---------------------------------------------------------------------------
void EventLoop::Wakeup()
{
    uint64_t    dat = 1;
    ssize_t     wlen= ::write(wakeupfd_, &dat, sizeof(dat));
    if(wlen != sizeof(dat))
    {
        SystemLog_Error("write failed");
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
void EventLoop::HandleRead()
{
    uint64_t    dat = 1;
    ssize_t     rlen= ::read(wakeupfd_, &dat, sizeof(dat));
    if(sizeof(dat) != rlen)
    {
        SystemLog_Error("read failed");
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
void EventLoop::DoPendingTasks()
{
    std::list<Task> task;
    is_pending_task_ = true;

    //交换内容,避免处理的时候长时间锁住task_list_
    {
    std::lock_guard<std::mutex> lock(mutex_);
    task.swap(task_list_);
    }

    for(auto iter=task.begin(); task.end()!=iter; ++iter)
    {
        (*iter)();
    }

    is_pending_task_ = false;
    return;
}
//---------------------------------------------------------------------------
}//namespace net
