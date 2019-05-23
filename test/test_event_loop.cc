//---------------------------------------------------------------------------
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/timerfd.h>
#include "test_inc.h"
#include "../src/event_loop.h"
#include "../thirdpart/base/include/thread.h"
#include "../src/channel.h"
#include "../src/timer.h"
#include "../src/timer_queue.h"
//---------------------------------------------------------------------------
namespace test
{

using namespace net;
//---------------------------------------------------------------------------
EventLoop* g_loop;
//---------------------------------------------------------------------------
static bool rcv_sig_usr1_;
static bool rcv_sig_usr2_;
static bool rcv_sig_int_;
static bool rcv_sig_quit_;
//---------------------------------------------------------------------------
void ThreadEventLoop()
{
    g_loop->Loop();
    g_loop->Quit();
}
//---------------------------------------------------------------------------
void ThreadEventLoop1()
{
    fprintf(stderr, "event loop:%u", base::CurrentThread::tid());

    EventLoop loop;
    g_loop = &loop;
    loop.Loop();
}
//---------------------------------------------------------------------------
void ThreadEventLoop2()
{
    EventLoop loop;
    loop.SetHandleSingnal();
    loop.Loop();
}
//---------------------------------------------------------------------------
void SigIntCallback()
{
    rcv_sig_int_ = true;
    kill(getpid(), SIGQUIT);
    std::cout << "recv int sig" << std::endl;
}
//---------------------------------------------------------------------------
void SigQuitCallback()
{
    rcv_sig_quit_ = true;
    std::cout << "recv quit sig" << std::endl;
    g_loop->Quit();
}
//---------------------------------------------------------------------------
void SigUsr1Callback()
{
    rcv_sig_usr1_ = true;
    kill(getpid(), SIGUSR2);
    std::cout << "recv usr1 sig" << std::endl;
}
//---------------------------------------------------------------------------
void SigUsr2Callback()
{
    rcv_sig_usr2_ = true;
    kill(getpid(), SIGINT);
    std::cout << "recv usr2 sig" << std::endl;
}
//---------------------------------------------------------------------------
bool TestEventLoop::Test_Channel()
{
    int timefd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK|TFD_CLOEXEC);
    EventLoop loop;
    Channel channel(&loop, timefd);
    channel.DisableAll();
    channel.DisableAll();
    channel.ReadDisable();
    channel.ReadDisable();
    channel.WriteDisable();
    channel.WriteDisable();
    MY_ASSERT(channel.IsNoneEvent());

    channel.ReadEnable();
    channel.ReadEnable();
    channel.WriteEnable();
    channel.WriteEnable();
    MY_ASSERT(!channel.IsNoneEvent());
    channel.ReadDisable();
    channel.ReadDisable();
    channel.WriteDisable();
    channel.DisableAll();
    channel.WriteDisable();
    channel.ReadEnable();
    channel.DisableAll();
    MY_ASSERT(channel.IsNoneEvent());

    channel.Remove();
    channel.Remove();

    return true;
}
//---------------------------------------------------------------------------
bool TestEventLoop::Test_Normal()
{
    //非法的,成功dump
    {
    //EventLoop loop;
    //g_loop = &loop;
    //base::Thread t(ThreadEventLoop);
    //t.Start();
    //t.Join();
    }

    //非法的设置处理信号线程,成功dump
    {
    //base::Thread t(ThreadEventLoop2);
    //t.Start();
    //t.Join();
    }

    //合法的
    {
    base::Thread t(ThreadEventLoop1);
    t.Start();
    sleep(2);
    g_loop->Quit();
    t.Join();
    }

    return true;
}
//---------------------------------------------------------------------------
void SigTimeout(base::Timestamp, int  timerfd)
{
    std::cout << "sig timeout" << std::endl;
    uint64_t dat;
    ssize_t rlen = ::read(timerfd, &dat, sizeof(dat));
    if(rlen != sizeof(dat))
    {
        assert(0);
    }

    kill(getpid(), SIGUSR1);
}
//---------------------------------------------------------------------------
bool TestEventLoop::Test_Signal()
{
    fprintf(stderr, "====>event loop:%u\n", base::CurrentThread::tid());

    EventLoop loop;
    loop.set_sig_int_callback(SigIntCallback);
    loop.set_sig_quit_callback(SigQuitCallback);
    loop.set_sig_usr1_callback(SigUsr1Callback);
    loop.set_sig_usr2_callback(SigUsr2Callback);
    loop.SetAsSignalHandleEventLoop();

    //添加多个Channel
    const int size = 8;
    int     fds[size];
    Channel* cl[size];
    for(int i=0; i<size; i++)
    {
        fds[i] = ::timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC|TFD_NONBLOCK);
        if(0 > fds[i])
            assert(0);

        cl[i] = new Channel(&loop, fds[i]);
        cl[i]->set_callback_read(std::bind(SigTimeout, base::Timestamp(5), fds[i]));
        cl[i]->ReadEnable();

        struct itimerspec howlog;
        bzero(&howlog, sizeof(howlog));
        howlog.it_value.tv_sec = 5;
        howlog.it_interval.tv_sec  = 0;
        howlog.it_interval.tv_nsec= 0;
        ::timerfd_settime(fds[i], 0, &howlog, NULL);
    }
    g_loop = &loop;
    loop.Loop();
    assert(true == rcv_sig_int_);
    assert(true == rcv_sig_usr1_);
    assert(true == rcv_sig_usr2_);
    assert(true == rcv_sig_quit_);
    for(int i=0; i<size; i++)
    {
        std::cout << "fd:" << cl[i]->fd() << std::endl;
        cl[i]->ReadDisable();
        cl[i]->Remove();
        delete cl[i];
        ::close(fds[i]);
    }

    return true;
}
//---------------------------------------------------------------------------
void RunInLoop()
{
    std::cout << "tid:" << base::CurrentThread::tid() 
        << " RunInLoop" << std::endl;
}
//---------------------------------------------------------------------------
void QueueInLoop()
{
    std::cout << "tid:" << base::CurrentThread::tid() 
        << " QueueInLoop" << std::endl;
}
bool TestEventLoop::Test_RunInLoop()
{
    std::cout << "=====>In current thread" << std::endl;
    EventLoop loop;
    loop.RunInLoop(RunInLoop);
    loop.RunInLoop(RunInLoop);
    loop.RunInLoop(RunInLoop);
    loop.QueueInLoop(QueueInLoop);
    loop.QueueInLoop(QueueInLoop);
    loop.QueueInLoop(QueueInLoop);
    //loop.Loop();

    return true;
}
//---------------------------------------------------------------------------
void Timeout(base::Timestamp rcv_time, int timerfd)
{
    uint64_t dat;
    ssize_t rlen = ::read(timerfd, &dat, sizeof(dat));
    if(rlen != sizeof(dat))
    {
        assert(0);
    }
    printf("Timeout:%s\n", rcv_time.Datetime().c_str());
    g_loop->Quit();
}
//---------------------------------------------------------------------------
bool TestEventLoop::Test_Timefd()
{
    EventLoop loop;
    g_loop = &loop;

    int timefd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK|TFD_CLOEXEC);
    Channel channel(&loop, timefd);
    channel.set_callback_read(std::bind(Timeout, base::Timestamp(5), timefd));
    channel.ReadEnable();

    struct itimerspec howlog;
    bzero(&howlog, sizeof(howlog));
    howlog.it_value.tv_sec = 5;
    howlog.it_interval.tv_sec  = 10;
    howlog.it_interval.tv_nsec= 0;
    ::timerfd_settime(timefd, 0, &howlog, NULL);

    loop.Loop();
    channel.DisableAll();
    channel.ReadDisable();
    channel.Remove();
    ::close(timefd);

    return true;
}
//---------------------------------------------------------------------------
bool g_flag = false;
int cat = 0;
int rat = 0;
int r = 0;
//---------------------------------------------------------------------------
void OnTimerTaskRunAt()
{
    printf("======================>OnTimerTaskRunAt:%d\n", cat++);
    g_flag = true;
}
//---------------------------------------------------------------------------
void OnTimerTaskRunAter()
{
    printf("===============================>OnTimerTaskRunAfter:%d\n", rat++);
    g_flag = true;
}
//---------------------------------------------------------------------------
void OnTimerTaskRunInterval()
{
    printf("==================================>OnTimerTaskRunInterval:%d\n", r++);
}
//---------------------------------------------------------------------------
void BeforLoop()
{
    static int count = 0;
    std::cout << "befor:" << count++ << std::endl;
}
//---------------------------------------------------------------------------
void AfterLoop()
{
    static int count = 0;
    std::cout << "after:" << count++ << std::endl;
}
//---------------------------------------------------------------------------
void OnTimerTask()
{
    g_flag = false;
    printf("thread start.......\n");
    uint64_t when = base::Timestamp::Now().AddTime(2).Microseconds();
    g_loop->RunAt(when, OnTimerTaskRunAt);
    g_loop->RunAt(when, OnTimerTaskRunAt);
    g_loop->RunAt(when, OnTimerTaskRunAt);
    g_loop->RunAt(when, OnTimerTaskRunAt);
    g_loop->RunAt(when, OnTimerTaskRunAt);
    g_loop->RunAt(when, OnTimerTaskRunAt);
    g_loop->RunAt(when, OnTimerTaskRunAt);
    g_loop->RunAt(when, OnTimerTaskRunAt);
    g_loop->RunAt(when, OnTimerTaskRunAt);
    g_loop->RunAt(when, OnTimerTaskRunAt);

    printf("sleep 5s.......\n");
    sleep(5);
    assert(true == g_flag);

    g_flag = false;
    g_loop->RunAfter(2, OnTimerTaskRunAter);
    g_loop->RunAfter(2, OnTimerTaskRunAter);
    g_loop->RunAfter(2, OnTimerTaskRunAter);
    g_loop->RunAfter(2, OnTimerTaskRunAter);
    g_loop->RunAfter(2, OnTimerTaskRunAter);
    g_loop->RunAfter(2, OnTimerTaskRunAter);
    g_loop->RunAfter(2, OnTimerTaskRunAter);
    g_loop->RunAfter(2, OnTimerTaskRunAter);
    g_loop->RunAfter(2, OnTimerTaskRunAter);
    g_loop->RunAfter(2, OnTimerTaskRunAter);
    sleep(5);
    assert(true == g_flag);
    
    TimerTaskId tid1 = g_loop->RunInterval(1, OnTimerTaskRunInterval);
    TimerTaskId tid2 = g_loop->RunInterval(1, OnTimerTaskRunInterval);
    sleep(10);
    g_loop->RunCancel(tid1);

    g_loop->Quit();
}
//---------------------------------------------------------------------------
bool TestEventLoop::Test_TimerTask()
{
    EventLoop loop;
    g_loop = &loop;
    loop.set_loop_befor_function(BeforLoop);
    loop.set_loop_after_function(AfterLoop);

    base::Thread t(OnTimerTask);
    t.Start();
    sleep(2);
    loop.Loop();
    t.Join();

    return true;
}
//---------------------------------------------------------------------------
int main(int, char**)
{
    TestTitle();

    TEST_ASSER(Test_Channel());
    TEST_ASSER(Test_Normal());
    TEST_ASSER(Test_Signal());
    TEST_ASSER(Test_RunInLoop());
    TEST_ASSER(Test_Timefd());
    TEST_ASSER(Test_TimerTask());

    return 0;
}
//---------------------------------------------------------------------------

}//namespace test
//---------------------------------------------------------------------------
