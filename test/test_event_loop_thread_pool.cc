//---------------------------------------------------------------------------
#include <unistd.h>
#include "test_inc.h"
#include "../src/event_loop_thread_pool.h"
#include "../src/event_loop.h"
#include "../thirdpart/base/include/thread.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
bool Test_TaskTimer()
{
    printf("pid:%d, tid=%d\n", getpid(), base::CurrentThread::tid());

    EventLoop main_loop;
    EventLoopThreadPool loop_thread_pool(&main_loop);
    loop_thread_pool.set_thread_nums(16);
    loop_thread_pool.Start();
    std::cout << "wait 5s initailze" << std::endl;
    sleep(5);
    loop_thread_pool.Stop();

    return true;
}
//---------------------------------------------------------------------------
int main()
{
    TestTitle();

    TEST_ASSERT(Test_TaskTimer());

    return 0;
}
//---------------------------------------------------------------------------
