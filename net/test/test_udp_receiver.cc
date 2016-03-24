//---------------------------------------------------------------------------
#include "test_udp_receiver.h"
#include "../udp_receiver.h"
#include "../event_loop.h"
#include "../callback.h"
#include "../inet_address.h"
#include "../packet_queue.h"
#include "../datagram_socket.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
namespace 
{
    const char* RCV_IP  = "127.0.0.1";
    short       RCV_PORT= 9999;
}
//---------------------------------------------------------------------------
bool TestUDPReceiver::DoTest()
{
    if(false == Test_Normal())  return false;

    return true;
}
//---------------------------------------------------------------------------
void OnError()
{
    std::cout << "catch error" << std::endl;
    return;
}
//---------------------------------------------------------------------------
bool TestUDPReceiver::Test_Normal()
{
    EventLoop       loop;
    InetAddress     rcv_addr(RCV_IP, RCV_PORT);
    DatagramSocket  sock(rcv_addr);
    PacketQueue     pkt_queue;
    UDPReceiver receiver(&loop, &sock, &pkt_queue);
    
    receiver.set_callback_error(OnError);
    receiver.Start();
    loop.Loop();
    receiver.Stop();

    return true;
}
//---------------------------------------------------------------------------
