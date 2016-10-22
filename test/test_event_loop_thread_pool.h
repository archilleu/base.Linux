//---------------------------------------------------------------------------
#ifndef LINUX_NET_TEST_EVENT_LOOP_THREAD_POOL_H_
#define LINUX_NET_TEST_EVENT_LOOP_THREAD_POOL_H_
//---------------------------------------------------------------------------
#include "test_base.h"
//---------------------------------------------------------------------------
namespace net
{

namespace test
{

class TestEventLoopThreadPool : public TestBase
{
public:
    TestEventLoopThreadPool()
    {
    }
    virtual ~TestEventLoopThreadPool()
    {
    }

   virtual bool DoTest();

private:
   bool Test_TaskTimer();
};

}//namespace test

}//namespace net
//---------------------------------------------------------------------------
#endif //LINUX_NET_TEST_EVENT_LOOP_THREAD_POOL_H_

