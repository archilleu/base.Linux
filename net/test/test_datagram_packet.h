//---------------------------------------------------------------------------
#ifndef LINUX_NET_TEST_DATAGRAM_PACKET_H_
#define LINUX_NET_TEST_DATAGRAM_PACKET_H_
//---------------------------------------------------------------------------
#include "test_base.h"
//---------------------------------------------------------------------------
namespace net
{

namespace test
{

class TestDatagramPacket : public TestBase
{
public:
    TestDatagramPacket()
    {
    }
    virtual ~TestDatagramPacket()
    {
    }

    virtual bool DoTest();

private:
    bool Test_Normal();
};

}//namespace test

}//namespace net
//---------------------------------------------------------------------------
#endif//LINUX_NET_TEST_DATAGRAM_PACKET_H_
