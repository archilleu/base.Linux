//---------------------------------------------------------------------------
#ifndef LINUX_NET_TEST_EVENT_LOOP_H_
#define LINUX_NET_TEST_EVENT_LOOP_H_
//---------------------------------------------------------------------------
#include "test_base.h"
//---------------------------------------------------------------------------
namespace net
{

namespace test
{

class TestEventLoop : public TestBase
{
public:
    TestEventLoop()
    {
    }
    virtual ~TestEventLoop()
    {
    }

    virtual bool DoTest();

private:
    bool Test_Channel();
    bool Test_Normal();
    bool Test_Signal();
    bool Test_RunInLoop();
    bool Test_Timefd();
    bool Test_TimerTask();
};

}//namespace test

}//namespace net
//---------------------------------------------------------------------------
#endif //LINUX_NET_TEST_EVENT_LOOP_H_
