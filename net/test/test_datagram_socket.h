//---------------------------------------------------------------------------
#ifndef LINUX_NET_TEST_DATAGRAM_SOCKET_H_
#define LINUX_NET_TEST_DATAGRAM_SOCKET_H_
//---------------------------------------------------------------------------
#include "test_base.h"
//---------------------------------------------------------------------------
namespace net
{

namespace test
{

class TestDatagramSocket : public TestBase
{
public:
    TestDatagramSocket()
    {
    }
    virtual ~TestDatagramSocket()
    {
    }

    virtual bool DoTest();

private:
    bool Test_Normal();
};

}//namespace test

}//namespace net
//---------------------------------------------------------------------------
#endif // LINUX_NET_TEST_DATAGRAM_SOCKET_H_
