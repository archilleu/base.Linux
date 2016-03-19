//---------------------------------------------------------------------------
#include "test_datagram_packet.h"
#include "../datagram_packet.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
bool TestDatagramPacket::DoTest()
{
    if(false == Test_Normal())  return false;

    return true;
}
//---------------------------------------------------------------------------
bool TestDatagramPacket::Test_Normal()
{
    {
    DatagramPacket pkt(10);
    MY_ASSERT(pkt.effective() == 0);
    MY_ASSERT(pkt.address() == InetAddress::INVALID_ADDR);
    }

    {
    DatagramPacket pkt(8);
    base::MemoryBlock mb = pkt.dat();
    bzero(mb.dat(), mb.len());

    pkt.set_effective(10);
    pkt.set_effective(8);


    }
    return true;
}
//---------------------------------------------------------------------------
