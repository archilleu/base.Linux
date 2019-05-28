//---------------------------------------------------------------------------
#include "test_inc.h"
#include "../src/udp_receiver.h"
#include "../src/event_loop.h"
#include "../src/callback.h"
#include "../src/inet_address.h"
#include "../src/packet_queue.h"
#include "../src/datagram_socket.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
namespace 
{
    short RCV_PORT= 9999;
    const char* RCV_IP = "127.0.0.1";
}
//---------------------------------------------------------------------------
void OnError()
{
    std::cout << "catch error" << std::endl;
    return;
}
//---------------------------------------------------------------------------
bool Test_Normal()
{
    EventLoop loop;
    InetAddress rcv_addr(RCV_IP, RCV_PORT);
    DatagramSocket sock(rcv_addr);
    PacketQueue pkt_queue;
    UDPReceiver receiver(&loop, &sock, &pkt_queue);
    
    receiver.set_callback_error(OnError);
    receiver.Start();
    loop.Loop();
    receiver.Stop();

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
