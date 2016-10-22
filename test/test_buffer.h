//---------------------------------------------------------------------------
#ifndef LINUX_NET_TEST_BUFFER_H_
#define LINUX_NET_TEST_BUFFER_H_
//---------------------------------------------------------------------------
#include "test_base.h"
//---------------------------------------------------------------------------
namespace net
{

namespace test
{

class TestBuffer : public TestBase
{
public:
    TestBuffer()
    {
    }
    virtual ~TestBuffer()
    {
    }

    virtual bool DoTest();

private:
    bool Test_Illegal();
    bool Test_Normal();
};

}//namespace test

}//namespace net
//---------------------------------------------------------------------------
#endif //LINUX_NET_TEST_BUFFER_H_

