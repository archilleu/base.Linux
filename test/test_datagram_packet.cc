//---------------------------------------------------------------------------
#include "test_datagram_packet.h"
#include "../src/datagram_packet.h"
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
    const int size = 1024*20;
    DatagramPacket pkt(size);
    net::MemoryBlock& mb = pkt.dat();
    bzero(mb.data(), mb.size());

    //pkt.set_effective(size*2);//崩溃好过隐藏错误
    pkt.set_effective(size);

    MY_ASSERT(size == pkt.effective());
    net::MemoryBlock cmp_buf(size, 0);
    
    MY_ASSERT(0 == memcmp(cmp_buf.data(), mb.data(), size));

    //各种赋值
    DatagramPacket pkt_copy(pkt);
    DatagramPacket pkt_assi = pkt;

    MY_ASSERT(pkt_copy.dat() == pkt.dat())
    MY_ASSERT(pkt_assi.dat() == pkt.dat())
    MY_ASSERT(pkt_copy.dat() == pkt_assi.dat())
    
    DatagramPacket pkt_move_copy(std::move(pkt_copy));
    DatagramPacket pkt_move_assi = std::move(pkt_assi);
    MY_ASSERT(pkt_move_copy.dat() == pkt.dat())
    MY_ASSERT(pkt_move_assi.dat() == pkt.dat())
    MY_ASSERT(pkt_move_copy.dat() == pkt_move_assi.dat())

    {
    net::MemoryBlock mb1(size, 'a');

    DatagramPacket pkt1(mb1);
    MY_ASSERT(pkt1.dat() == mb1);

    DatagramPacket pkt2(std::move(mb1));
    MY_ASSERT(pkt1.dat() == pkt2.dat());

    InetAddress addr("127.0.0.1", 8000);
    DatagramPacket pkt3(size, addr);
    MY_ASSERT(pkt3.address() == addr);
    MY_ASSERT(pkt3.dat().size() == size);
    }

    return true;
    }
}
//---------------------------------------------------------------------------
