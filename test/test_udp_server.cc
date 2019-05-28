//---------------------------------------------------------------------------
#include "test_inc.h"
#include "../src/udp_server.h"
#include "../src/inet_address.h"
#include "../src/datagram_packet.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
namespace 
{
    const char* SERVER_IP = "127.0.0.1";
    const short SERVER_PORT = 9999;

    UDPServer* g_udp_server = 0;
}
//---------------------------------------------------------------------------
void OnRcvPacket(const DatagramPacket& pkt)
{
    static uint64_t times = 0;
    
    std::cout << "times:" << ++times << " from:" << pkt.address().IpPort() << " len:" << pkt.dat().size() << " effective:" << pkt.effective() << std::endl;
    
    g_udp_server->Send(pkt);
    return;
}
//---------------------------------------------------------------------------
bool Test_Normal()
{
    InetAddress rcv_addr(SERVER_IP, SERVER_PORT);
    UDPServer udp_server(rcv_addr);
    g_udp_server = &udp_server;

    udp_server.set_callback_rcv_pkt(std::bind(OnRcvPacket, std::placeholders::_1));
    udp_server.Start(8);
    udp_server.Stop();
    return true;
}
//---------------------------------------------------------------------------
int main()
{
    TestTitle();

    TEST_ASSERT(Test_Normal());

    return 0;
}
//---------------------------------------------------------------------------
