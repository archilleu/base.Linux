//---------------------------------------------------------------------------
#include "test_event_loop_thread.h"
#include "../event_loop_thread.h"
#include "../event_loop.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
bool TestEventLoopThread::DoTest()
{
    if(false == Test_TaskTimer())   return false;

    return true;
}
//---------------------------------------------------------------------------
void RunTimerTask()
{
    printf("pid:%d, tid=%d\n", getpid(), base::CurrentThread::tid());
    printf("run timer task\n");
}
bool TestEventLoopThread::Test_TaskTimer()
{
    printf("pid:%d, tid=%d\n", getpid(), base::CurrentThread::tid());

    EventLoopThread loop_thread;
    EventLoop*      loop =  loop_thread.StartLoop();
    loop->RunInLoop(RunTimerTask);
    sleep(1);
    loop->RunAfter(2, RunTimerTask);
    sleep(5);
    loop_thread.StopLoop();

    return true;
}
//---------------------------------------------------------------------------
