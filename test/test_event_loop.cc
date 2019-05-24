//---------------------------------------------------------------------------
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/timerfd.h>
#include "test_inc.h"
#include "../src/event_loop.h"
#include "../thirdpart/base/include/thread.h"
#include "../src/channel.h"
#include "../src/timer_id.h"
#include "../src/timer_queue.h"
//---------------------------------------------------------------------------
using namespace net;
//---------------------------------------------------------------------------
EventLoop* g_loop;
//---------------------------------------------------------------------------
static bool g_rcv_sig_usr1;
static bool g_rcv_sig_usr2;
static bool g_rcv_sig_int;
static bool g_rcv_sig_quit_;
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
    g_rcv_sig_int = true;
    kill(getpid(), SIGQUIT);
    std::cout << "recv int sig" << std::endl;
}
//---------------------------------------------------------------------------
void SigQuitCallback()
{
    g_rcv_sig_quit_ = true;
    std::cout << "recv quit sig" << std::endl;
    g_loop->Quit();
}
//---------------------------------------------------------------------------
void SigUsr1Callback()
{
    g_rcv_sig_usr1 = true;
    kill(getpid(), SIGUSR2);
    std::cout << "recv usr1 sig" << std::endl;
}
//---------------------------------------------------------------------------
void SigUsr2Callback()
{
    g_rcv_sig_usr2 = true;
    kill(getpid(), SIGINT);
    std::cout << "recv usr2 sig" << std::endl;
}
//---------------------------------------------------------------------------
bool Test_Channel()
{
    int timefd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK|TFD_CLOEXEC);
    EventLoop loop;
    Channel channel(&loop, timefd);
    channel.DisableAll();
    channel.EnableReading();
    channel.DisableReading();
    channel.DisableWriting();
    TEST_ASSERT(channel.IsNoneEvent());

    channel.EnableReading();
    channel.EnableWriting();
    TEST_ASSERT(!channel.IsNoneEvent());
    channel.DisableReading();
    channel.DisableWriting();
    channel.DisableAll();
    channel.DisableWriting();
    channel.EnableReading();
    channel.DisableAll();
    TEST_ASSERT(channel.IsNoneEvent());

    channel.Remove();
    channel.Remove();

    return true;
}
//---------------------------------------------------------------------------
bool Test_Normal()
{
    //非法的,成功dump(要在主线程启动)
    {
    //EventLoop loop;
    //g_loop = &loop;
    //base::Thread t(ThreadEventLoop);
    //t.Start();
    //t.Join();
    }

    //非法的设置处理信号线程,成功dump(只能在主线程设置信号处理)
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
bool Test_Signal()
{
    fprintf(stderr, "====>event loop:%u\n", base::CurrentThread::tid());

    EventLoop loop;
    loop.set_sig_int_cb(SigIntCallback);
    loop.set_sig_quit_cb(SigQuitCallback);
    loop.set_sig_usr1_cb(SigUsr1Callback);
    loop.set_sig_usr2_cb(SigUsr2Callback);
    loop.SetHandleSingnal();

    //添加多个Channel
    const int size = 8;
    int fds[size];
    Channel* cl[size];
    for(int i=0; i<size; i++)
    {
        fds[i] = ::timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC|TFD_NONBLOCK);
        if(0 > fds[i])
            assert(0);

        cl[i] = new Channel(&loop, fds[i]);
        cl[i]->set_read_cb(std::bind(SigTimeout, base::Timestamp(5), fds[i]));
        cl[i]->EnableReading();

        struct itimerspec howlog;
        bzero(&howlog, sizeof(howlog));
        howlog.it_value.tv_sec = 5;
        howlog.it_interval.tv_sec  = 0;
        howlog.it_interval.tv_nsec= 0;
        ::timerfd_settime(fds[i], 0, &howlog, NULL);
    }
    g_loop = &loop;
    loop.Loop();
    assert(true == g_rcv_sig_int);
    assert(true == g_rcv_sig_usr1);
    assert(true == g_rcv_sig_usr2);
    assert(true == g_rcv_sig_quit_);
    for(int i=0; i<size; i++)
    {
        std::cout << "fd:" << cl[i]->fd() << std::endl;
        cl[i]->DisableReading();
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
//---------------------------------------------------------------------------
bool Test_RunInLoop()
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
bool Test_Timefd()
{
    EventLoop loop;
    g_loop = &loop;

    int timefd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK|TFD_CLOEXEC);
    Channel channel(&loop, timefd);
    channel.set_read_cb(std::bind(Timeout, base::Timestamp(5), timefd));
    channel.EnableReading();

    struct itimerspec howlog;
    bzero(&howlog, sizeof(howlog));
    howlog.it_value.tv_sec = 5;
    howlog.it_interval.tv_sec  = 10;
    howlog.it_interval.tv_nsec= 0;
    ::timerfd_settime(timefd, 0, &howlog, NULL);

    loop.Loop();
    channel.DisableAll();
    channel.DisableReading();
    channel.Remove();
    ::close(timefd);

    return true;
}
//---------------------------------------------------------------------------
bool g_flag = false;
//---------------------------------------------------------------------------
void OnTimerTaskRunAt()
{
    static int cat = 0;
    printf("======================>OnTimerTaskRunAt:%d\n", cat++);
    g_flag = true;
}
//---------------------------------------------------------------------------
void OnTimerTaskRunAter()
{
    static int rat = 0;
    printf("===============================>OnTimerTaskRunAfter:%d\n", rat++);
    g_flag = true;
}
//---------------------------------------------------------------------------
void OnTimerTaskRunInterval()
{
    static int r = 0;
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
    base::Timestamp when = base::Timestamp::Now().AddTime(2);
    for(int i=0; i<10; i++)
    {
        g_loop->TimerAt(when, OnTimerTaskRunAt);
    }

    printf("sleep 5s.......\n");
    sleep(5);
    assert(true == g_flag);

    g_flag = false;
    for(int i=0; i<10; i++)
    {
        g_loop->TimerAfter(2, OnTimerTaskRunAter);
    }
    sleep(5);
    assert(true == g_flag);
    
    TimerId tid1 = g_loop->TimerInterval(1, OnTimerTaskRunInterval);
    TimerId tid2 = g_loop->TimerInterval(1, OnTimerTaskRunInterval);
    sleep(10);
    g_loop->TimerCancel(tid1);
    g_loop->TimerCancel(tid2);

    g_loop->Quit();
}
//---------------------------------------------------------------------------
bool Test_TimerTask()
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
    base::test::TestTitle();

    TEST_ASSERT(Test_Channel());
    TEST_ASSERT(Test_Normal());
    TEST_ASSERT(Test_Signal());
    TEST_ASSERT(Test_RunInLoop());
    TEST_ASSERT(Test_Timefd());
    TEST_ASSERT(Test_TimerTask());

    return 0;
}
//---------------------------------------------------------------------------
