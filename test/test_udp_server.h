//---------------------------------------------------------------------------
#ifndef LINUX_NET_TEST_UDP_SERVER_H_
#define LINUX_NET_TEST_UDP_SERVER_H_
//---------------------------------------------------------------------------
#include "test_base.h"
#include "../src/datagram_packet.h"
//---------------------------------------------------------------------------
namespace net
{

namespace test
{

class TestUDPServer : public TestBase
{
public:
    TestUDPServer()
    {
    }
    virtual ~TestUDPServer()
    {
    }

    virtual bool DoTest();

private:
    void OnRcvPacket(const DatagramPacket& pkt);

private:
    bool Test_Normal();
};

}//namespace test

}//namespace net
//---------------------------------------------------------------------------
#endif //LINUX_NET_TEST_UDP_SERVER_H_
