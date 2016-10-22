//---------------------------------------------------------------------------
#include "test_datagram_socket.h"
#include "../src/datagram_socket.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
namespace
{
    const char* svr_ip  = "127.0.0.1";
    const char* cli_ip  = svr_ip;
    const short svr_port= 11111;
    const short cli_port= 10000;
}
//---------------------------------------------------------------------------
bool TestDatagramSocket::DoTest()
{
    if(false == Test_Anonym())  return false;
    if(false == Test_Named())   return false;
    if(false == Test_Normal())  return false;

    return true;
}
//---------------------------------------------------------------------------
bool TestDatagramSocket::Test_Anonym()
{
    DatagramSocket ds;
    MY_ASSERT(false == ds.IsBind());
    MY_ASSERT(false == ds.IsConnect());

    MY_ASSERT(InetAddress::INVALID_ADDR == ds.server_addr());
    std::cout << "local:" << ds.local_addr().IPPort() << std::endl;
    std::cout << "is broadcast " << ds.IsBroadcast() << std::endl;

    std::cout << "rcv buf size: " << ds.GetRcvBufferSize() << "snd buf size:" << ds.GetSndBufferSize() << std::endl;
    ds.SetRcvBufferSize(1024*100);
    ds.SetSndBufferSize(1024*100);
    std::cout << "rcv buf size: " << ds.GetRcvBufferSize() << "snd buf size:" << ds.GetSndBufferSize() << std::endl;

    return true;
}
//---------------------------------------------------------------------------
bool TestDatagramSocket::Test_Named()
{
    {
    DatagramSocket ds(cli_port);
    ds.SetReuseAddress();

    MY_ASSERT(true == ds.IsBind());
    MY_ASSERT(false == ds.IsConnect());

    MY_ASSERT(InetAddress::INVALID_ADDR == ds.server_addr());
    std::cout << "local:" << ds.local_addr().IPPort() << std::endl;
    std::cout << "is broadcast " << ds.IsBroadcast() << std::endl;

    std::cout << "rcv buf size: " << ds.GetRcvBufferSize() << "snd buf size:" << ds.GetSndBufferSize() << std::endl;
    ds.SetRcvBufferSize(1024*100);
    ds.SetSndBufferSize(1024*100);
    std::cout << "rcv buf size: " << ds.GetRcvBufferSize() << "snd buf size:" << ds.GetSndBufferSize() << std::endl;
    }

    {
    DatagramSocket ds(InetAddress(cli_ip, cli_port));
    ds.SetReuseAddress();

    MY_ASSERT(true == ds.IsBind());
    MY_ASSERT(false == ds.IsConnect());

    MY_ASSERT(InetAddress::INVALID_ADDR == ds.server_addr());
    std::cout << "local:" << ds.local_addr().IPPort() << std::endl;
    std::cout << "is broadcast " << ds.IsBroadcast() << std::endl;

    std::cout << "rcv buf size: " << ds.GetRcvBufferSize() << "snd buf size:" << ds.GetSndBufferSize() << std::endl;
    ds.SetRcvBufferSize(1024*100);
    ds.SetSndBufferSize(1024*100);
    std::cout << "rcv buf size: " << ds.GetRcvBufferSize() << "snd buf size:" << ds.GetSndBufferSize() << std::endl;
    }

    return true;
}
//---------------------------------------------------------------------------
bool TestDatagramSocket::Test_Normal()
{
    //没有connect
    {
    DatagramSocket ds;
    for(uint64_t i=0; i<1024; i++)
    {
        DatagramPacket pkt(rand()%65536, InetAddress(svr_ip, svr_port));
        memset(pkt.dat().dat(), rand()%256, pkt.dat().len());
        pkt.set_effective(pkt.dat().len());

        ds.Send(pkt);
        DatagramPacket pkt_rcv = ds.Receive(65536);
        MY_ASSERT(0 == memcmp(pkt_rcv.dat().dat(), pkt.dat().dat(), pkt.effective()));
    }
    }

    {
    DatagramSocket ds;
    ds.Connect(InetAddress(svr_ip, svr_port));
    MY_ASSERT(true == ds.IsConnect());
    for(uint64_t i=0; i<1024; i++)
    {
        DatagramPacket pkt(rand()%65536);
        memset(pkt.dat().dat(), rand()%256, pkt.dat().len());
        pkt.set_effective(pkt.dat().len());

        ds.Send(pkt);
        DatagramPacket pkt_rcv = ds.Receive(65536);
        MY_ASSERT(0 == memcmp(pkt_rcv.dat().dat(), pkt.dat().dat(), pkt.effective()));
    }
    }

    {
    DatagramSocket ds;
    ds.Bind(InetAddress(cli_ip, cli_port));
    ds.Connect(InetAddress(svr_ip, svr_port));
    MY_ASSERT(true == ds.IsBind());
    MY_ASSERT(true == ds.IsConnect());
    MY_ASSERT(InetAddress(cli_ip, cli_port) == ds.local_addr());
    for(uint64_t i=0; i<1024; i++)
    {
        DatagramPacket pkt(rand()%65536);
        memset(pkt.dat().dat(), rand()%256, pkt.dat().len());
        pkt.set_effective(pkt.dat().len());

        ds.Send(pkt);
        DatagramPacket pkt_rcv = ds.Receive(65536);
        MY_ASSERT(0 == memcmp(pkt_rcv.dat().dat(), pkt.dat().dat(), pkt.effective()));
    }
    }
    return true;
}
//---------------------------------------------------------------------------
