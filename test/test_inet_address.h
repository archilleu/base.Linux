//---------------------------------------------------------------------------
#ifndef LINUX_NET_TEST_INET_ADDRESS_H_
#define LINUX_NET_TEST_INET_ADDRESS_H_
//---------------------------------------------------------------------------
#include "test_base.h"
//---------------------------------------------------------------------------
namespace net
{

namespace test
{

class TestInetAddress : public TestBase
{
public:
    TestInetAddress()
    {
    }
    virtual ~TestInetAddress()
    {
    }

    virtual bool DoTest();

private:
    bool Test_Normal();
};

}//namespace test

}//namespace net
//---------------------------------------------------------------------------
#endif //LINUX_NET_TEST_INET_ADDRESS_H_
