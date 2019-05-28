//---------------------------------------------------------------------------
#include "test_inc.h"
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
bool Test_Anonym()
{
    DatagramSocket ds;
    TEST_ASSERT(false == ds.IsBind());
    TEST_ASSERT(false == ds.IsConnect());

    TEST_ASSERT(InetAddress::INVALID_ADDR == ds.server_addr());
    std::cout << "local:" << ds.local_addr().IpPort() << std::endl;
    std::cout << "is broadcast " << ds.IsBroadcast() << std::endl;

    std::cout << "rcv buf size: " << ds.GetRcvBufferSize() << "snd buf size:" << ds.GetSndBufferSize() << std::endl;
    ds.SetRcvBufferSize(1024*100);
    ds.SetSndBufferSize(1024*100);
    std::cout << "rcv buf size: " << ds.GetRcvBufferSize() << "snd buf size:" << ds.GetSndBufferSize() << std::endl;

    return true;
}
//---------------------------------------------------------------------------
bool Test_Named()
{
    {
    DatagramSocket ds(cli_port);
    ds.SetReuseAddress();

    TEST_ASSERT(true == ds.IsBind());
    TEST_ASSERT(false == ds.IsConnect());

    TEST_ASSERT(InetAddress::INVALID_ADDR == ds.server_addr());
    std::cout << "local:" << ds.local_addr().IpPort() << std::endl;
    std::cout << "is broadcast " << ds.IsBroadcast() << std::endl;

    std::cout << "rcv buf size: " << ds.GetRcvBufferSize() << "snd buf size:" << ds.GetSndBufferSize() << std::endl;
    ds.SetRcvBufferSize(1024*100);
    ds.SetSndBufferSize(1024*100);
    std::cout << "rcv buf size: " << ds.GetRcvBufferSize() << "snd buf size:" << ds.GetSndBufferSize() << std::endl;
    }

    {
    DatagramSocket ds(InetAddress(cli_ip, cli_port));
    ds.SetReuseAddress();

    TEST_ASSERT(true == ds.IsBind());
    TEST_ASSERT(false == ds.IsConnect());

    TEST_ASSERT(InetAddress::INVALID_ADDR == ds.server_addr());
    std::cout << "local:" << ds.local_addr().IpPort() << std::endl;
    std::cout << "is broadcast " << ds.IsBroadcast() << std::endl;

    std::cout << "rcv buf size: " << ds.GetRcvBufferSize() << "snd buf size:" << ds.GetSndBufferSize() << std::endl;
    ds.SetRcvBufferSize(1024*100);
    ds.SetSndBufferSize(1024*100);
    std::cout << "rcv buf size: " << ds.GetRcvBufferSize() << "snd buf size:" << ds.GetSndBufferSize() << std::endl;
    }

    return true;
}
//---------------------------------------------------------------------------
bool Test_Normal()
{
    //没有connect
    {
    DatagramSocket ds;
    for(uint64_t i=0; i<1024; i++)
    {
        DatagramPacket pkt(rand()%65536, InetAddress(svr_ip, svr_port));
        memset(pkt.dat().data(), rand()%256, pkt.dat().size());
        pkt.set_effective(pkt.dat().size());

        ds.Send(pkt);
        DatagramPacket pkt_rcv = ds.Receive(65536);
        TEST_ASSERT(0 == memcmp(pkt_rcv.dat().data(), pkt.dat().data(), pkt.effective()));
    }
    }

    {
    DatagramSocket ds;
    ds.Connect(InetAddress(svr_ip, svr_port));
    TEST_ASSERT(true == ds.IsConnect());
    for(uint64_t i=0; i<1024; i++)
    {
        DatagramPacket pkt(rand()%65536);
        memset(pkt.dat().data(), rand()%256, pkt.dat().size());
        pkt.set_effective(pkt.dat().size());

        ds.Send(pkt);
        DatagramPacket pkt_rcv = ds.Receive(65536);
        TEST_ASSERT(0 == memcmp(pkt_rcv.dat().data(), pkt.dat().data(), pkt.effective()));
    }
    }

    {
    DatagramSocket ds;
    ds.Bind(InetAddress(cli_ip, cli_port));
    ds.Connect(InetAddress(svr_ip, svr_port));
    TEST_ASSERT(true == ds.IsBind());
    TEST_ASSERT(true == ds.IsConnect());
    TEST_ASSERT(InetAddress(cli_ip, cli_port) == ds.local_addr());
    for(uint64_t i=0; i<1024; i++)
    {
        DatagramPacket pkt(rand()%65536);
        memset(pkt.dat().data(), rand()%256, pkt.dat().size());
        pkt.set_effective(pkt.dat().size());

        ds.Send(pkt);
        DatagramPacket pkt_rcv = ds.Receive(65536);
        TEST_ASSERT(0 == memcmp(pkt_rcv.dat().data(), pkt.dat().data(), pkt.effective()));
    }
    }
    return true;
}
//---------------------------------------------------------------------------
int main()
{
    TEST_ASSERT(Test_Anonym());
    TEST_ASSERT(Test_Named());
    TEST_ASSERT(Test_Normal());

    return 0;
}
//---------------------------------------------------------------------------
