//---------------------------------------------------------------------------
#ifndef LINUX_NET_TEST_TCP_CLIENT_H_
#define LINUX_NET_TEST_TCP_CLIENT_H_
//---------------------------------------------------------------------------
#include "test_base.h"
#include "../callback.h"
//---------------------------------------------------------------------------
namespace net
{

namespace test
{

class TestTCPClient : public TestBase
{
public:
    TestTCPClient()
    {
    }
    virtual ~TestTCPClient()
    {
    }

    virtual bool DoTest();

private:
    bool Test_Normal();
};

}//namespace test;

}//namespace net
//---------------------------------------------------------------------------
#endif //LINUX_NET_TEST_TCP_CLIENT_H_
