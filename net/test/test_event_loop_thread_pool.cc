//---------------------------------------------------------------------------
#include "test_event_loop_thread_pool.h"
#include "../event_loop_thread_pool.h"
#include "../event_loop.h"
#include "../../base/thread.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
bool TestEventLoopThreadPool::DoTest()
{
    if(false == Test_TaskTimer())   return false;

    return true;
}
//---------------------------------------------------------------------------
bool TestEventLoopThreadPool::Test_TaskTimer()
{
    printf("pid:%d, tid=%d\n", getpid(), base::CurrentThread::tid());

    EventLoop main_loop;
    EventLoopThreadPool loop_thread_pool(&main_loop);
    loop_thread_pool.set_thread_nums(16);
    loop_thread_pool.Start();
    loop_thread_pool.Stop();

    return true;
}
//---------------------------------------------------------------------------
