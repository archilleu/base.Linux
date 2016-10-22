//---------------------------------------------------------------------------
#ifndef LINUX_NET_TEST_UDP_RECEIVER_H_
#define LINUX_NET_TEST_UDP_RECEIVER_H_
//---------------------------------------------------------------------------
#include "test_base.h"
#include "../src/callback.h"
//---------------------------------------------------------------------------
namespace net
{

namespace test
{

class TestUDPReceiver : public TestBase
{
public:
    TestUDPReceiver()
    {
    }
    virtual ~TestUDPReceiver()
    {
    }

    virtual bool DoTest();

private:
    bool Test_Normal();
};

}//namespace test

}//namespace net
//---------------------------------------------------------------------------
#endif //LINUX_NET_TEST_UDP_RECEIVER_H_
