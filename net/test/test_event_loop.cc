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
void ThreadEventLoop1()
{
    fprintf(stderr, "event loop:%u", base::CurrentThread::Tid());

    EventLoop loop;
    loop.Loop();
    sleep(1);
    loop.Quit();
}
//---------------------------------------------------------------------------
bool TestEventLoop::DoTest()
{
//    if(false == Test_Normal())  return false;//需要额外线程打断该测试
//    if(false == Test_Timefd())      return false;
    if(false == Test_TimerTask())   return false;

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

    //合法的
    {
    fprintf(stderr, "====>event loop:%u\n", base::CurrentThread::Tid());

    EventLoop loop;
    loop.Loop();
    loop.Quit();
    base::Thread t(ThreadEventLoop1);
    t.Start();
    t.Join();
    }

    return true;
}
//---------------------------------------------------------------------------
void Timeout(base::Timestamp rcv_time)
{
    printf("Timeout:%s\n", rcv_time.Datatime().c_str());
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
    loop.Loop();
    t.Join();

    return true;
}
//---------------------------------------------------------------------------
