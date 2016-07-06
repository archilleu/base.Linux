//---------------------------------------------------------------------------
#include "test_event_loop.h"
#include <sys/timerfd.h>
#include "../event_loop.h"
#include "../../base/thread.h"
#include "../channel.h"
#include "../timer_task.h"
#include "../timer_task_queue.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace test;
//---------------------------------------------------------------------------
EventLoop* g_loop;
//---------------------------------------------------------------------------
void ThreadEventLoop()
{
    g_loop->Loop();
    g_loop->Quit();
}
//---------------------------------------------------------------------------
void ThreadEventLoop2()
{
    EventLoop loop;
    loop.SetAsSignalHandleEventLoop();
    loop.Loop();
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
bool TestEventLoop::DoTest()
{
    if(false == Test_Normal())      return false;//需要额外线程打断该测试
    if(false == Test_Timefd())      return false;
    if(false == Test_TimerTask())   return false;

    return true;
}
//---------------------------------------------------------------------------
void SigIntCallback()
{
    std::cout << "recv int sig" << std::endl;
}
//---------------------------------------------------------------------------
void SigQuitCallback()
{
    std::cout << "recv quit sig" << std::endl;
    g_loop->Quit();
}
//---------------------------------------------------------------------------
void SigUsr1Callback()
{
    std::cout << "recv usr1 sig" << std::endl;
}
//---------------------------------------------------------------------------
void SigUsr2Callback()
{
    std::cout << "recv usr2 sig" << std::endl;
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
    fprintf(stderr, "====>event loop:%u\n", base::CurrentThread::tid());

    EventLoop loop;
    loop.set_sig_int_callback(SigIntCallback);
    loop.set_sig_quit_callback(SigQuitCallback);
    loop.set_sig_usr1_callback(SigUsr1Callback);
    loop.set_sig_usr2_callback(SigUsr2Callback);
    loop.SetAsSignalHandleEventLoop();

    //添加多个Channel
    const int size = 128;
    int     fds[size];
    Channel* cl[size];
    for(int i=0; i<size; i++)
    {
        fds[i] = ::timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC|TFD_NONBLOCK);
        if(0 > fds[i])
            assert(0);

        cl[i] = new Channel(&loop, fds[i]);
        cl[i]->ReadEnable();
    }
    g_loop = &loop;
    loop.Loop();
    for(int i=0; i<size; i++)
    {
        std::cout << "fd:" << cl[i]->fd() << std::endl;
        cl[i]->ReadDisable();
        cl[i]->Remove();
        delete cl[i];
        ::close(fds[i]);
    }

    base::Thread t(ThreadEventLoop1);
    t.Start();
    sleep(2);
    g_loop->Quit();
    t.Join();
    }

    return true;
}
//---------------------------------------------------------------------------
void Timeout(base::Timestamp rcv_time)
{
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
    channel.set_callback_read(std::bind(Timeout, base::Timestamp(5)));
    channel.ReadEnable();

    struct itimerspec howlog;
    bzero(&howlog, sizeof(howlog));
    howlog.it_value.tv_sec = 5;
    ::timerfd_settime(timefd, 0, &howlog, NULL);

    loop.Loop();
    channel.Remove();
    ::close(timefd);

    return true;
}
//---------------------------------------------------------------------------
bool g_flag = false;
//---------------------------------------------------------------------------
void OnTimerTaskRunAt()
{
    printf("OnTimerTaskRunAt\n");
    g_flag = true;
}
//---------------------------------------------------------------------------
void OnTimerTaskRunAter()
{
    printf("OnTimerTaskRunAfter\n");
    g_flag = true;
}
//---------------------------------------------------------------------------
void OnTimerTaskRunInterval()
{
    printf("OnTimerTaskRunInterval\n");
}
//---------------------------------------------------------------------------
void OnTimerTask()
{
    g_flag = false;
    printf("thread start.......\n");
    base::Timestamp when = base::Timestamp::Now().AddTime(2);
    g_loop->RunAt(when, OnTimerTaskRunAt);

    sleep(5);
    assert(true == g_flag);

    g_flag = false;
    g_loop->RunAfter(2, OnTimerTaskRunAter);
    sleep(5);
    assert(true == g_flag);
    
    TimerTask::Ptr ptr = g_loop->RunInterval(1, OnTimerTaskRunInterval);
    sleep(10);
    g_loop->RunCancel(ptr);

    g_loop->Quit();
}
//---------------------------------------------------------------------------
bool TestEventLoop::Test_TimerTask()
{
    EventLoop loop;
    g_loop = &loop;

    base::Thread t(OnTimerTask);
    t.Start();
    sleep(2);
    loop.Loop();
    t.Join();

    return true;
}
//---------------------------------------------------------------------------
