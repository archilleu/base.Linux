//---------------------------------------------------------------------------
#ifndef LINUX_NET_TEST_EVENT_LOOP_THREAD_H_
#define LINUX_NET_TEST_EVENT_LOOP_THREAD_H_
//---------------------------------------------------------------------------
#include "test_base.h"
//---------------------------------------------------------------------------
namespace net
{

namespace test
{

class TestEventLoopThread : public TestBase
{
public:
    TestEventLoopThread()
    {
    }
    virtual ~TestEventLoopThread()
    {
    }

   virtual bool DoTest();

private:
   bool Test_TaskTimer();
};

}//namespace test

}//namespace net
//---------------------------------------------------------------------------
#endif //LINUX_NET_TEST_EVENT_LOOP_THREAD_H_

