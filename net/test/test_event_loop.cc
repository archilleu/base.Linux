//---------------------------------------------------------------------------
#include "test_event_loop.h"
#include "../event_loop.h"
#include "../../base/thread.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace test;
//---------------------------------------------------------------------------
EventLoop* g_loop;
//---------------------------------------------------------------------------
void ThreadEventLoop()
{
    g_loop->Loop();
}
void ThreadEventLoop1()
{
    fprintf(stderr, "event loop:%u", base::CurrentThread::Tid());

    EventLoop loop;
    loop.Loop();
}
//---------------------------------------------------------------------------
bool TestEventLoop::DoTest()
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

    base::Thread t(ThreadEventLoop1);
    t.Start();
    t.Join();
    }
    return true;
}
//---------------------------------------------------------------------------
