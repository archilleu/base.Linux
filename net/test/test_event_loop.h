//---------------------------------------------------------------------------
#ifndef LINUX_NET_TEST_EVENT_LOOP_H_
#define LINUX_NET_TEST_EVENT_LOOP_H_
//---------------------------------------------------------------------------
#include "test_base.h"
//---------------------------------------------------------------------------
#define MY_ASSERT(EXPRESSION) {if(true != (EXPRESSION)) { assert(0); return false;}}
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
    bool Test_Normal();
    bool Test_Timefd();
};

}//namespace test

}//namespace net
//---------------------------------------------------------------------------
#endif //LINUX_NET_TEST_EVENT_LOOP_H_
