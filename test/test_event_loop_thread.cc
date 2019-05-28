//---------------------------------------------------------------------------
#include "test_inc.h"
#include "../src/event_loop_thread.h"
#include "../src/event_loop.h"
#include "../src/timer_id.h"
#include <cstring>
#include <unistd.h>
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
void RunTimerTask()
{
    printf("pid:%d, tid=%d\n", getpid(), base::CurrentThread::tid());
    printf("run timer task\n");
}
bool Test_TaskTimer()
{
    printf("pid:%d, tid=%d\n", getpid(), base::CurrentThread::tid());

    EventLoopThread loop_thread;
    EventLoop* loop = loop_thread.StartLoop();
    loop->RunInLoop(RunTimerTask);
    sleep(1);
    loop->TimerAfter(2, RunTimerTask);
    sleep(5);
    loop_thread.StopLoop();

    return true;
}
//---------------------------------------------------------------------------
int main()
{
    TEST_ASSERT(Test_TaskTimer());

    return 0;
}
//---------------------------------------------------------------------------
