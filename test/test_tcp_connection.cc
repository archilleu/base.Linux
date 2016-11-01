//---------------------------------------------------------------------------
#include <unistd.h>
#include "test_tcp_connection.h"
#include "../src/event_loop.h"
#include "../src/event_loop_thread_pool.h"
#include "../src/tcp_server.h"
#include "../src/tcp_connection.h"
#include "../src/acceptor.h"
#include "../src/buffer.h"
#include "../src/net_log.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
namespace
{

class Decoder
{
public:
    struct Header
    {
        int dat_len;
        int type;

        static const int kReply = 1;
        static const int kNotify= 2;
    };

    static int Decode(Buffer& buffer)
    {
        assert(8 == sizeof(Header));
        if(sizeof(Header) > buffer.ReadableBytes())
            return 0;

        Header header  = *reinterpret_cast<const Header*>(buffer.Peek());
        header.dat_len = be32toh(header.dat_len);
        header.type    = be32toh(header.type);
        if((sizeof(Header)+header.dat_len) > buffer.ReadableBytes())
            return 0;

        buffer.Retrieve(sizeof(Header));
        return header.dat_len;
    }

    static Header MakeHeader(int len, int type)
    {
        Header header;
        header.dat_len  = htobe32(len);
        header.type     = htobe32(type);
        return header;
    }
};

}
namespace
{
EventLoop* g_loop;

void Quit()
{
    g_loop->Quit();
    return;
}
}
//---------------------------------------------------------------------------
bool TestTCPConnection::DoTest()
{
    if(false == Test_Illegal())     return false;
    if(false == Test_Normal())      return false;
    if(false == Test_MultiThread()) return false;

    return true;
}
//---------------------------------------------------------------------------
bool TestTCPConnection::Test_Illegal()
{
    return true;
}
//---------------------------------------------------------------------------
bool TestTCPConnection::Test_Normal()
{
    EventLoop::SetLogger("/tmp/logger", EventLoop::ERROR);
    EventLoop loop;
    g_loop = &loop;
    TCPServer   server(&loop, 9999);
    loop.set_sig_quit_callback(Quit);
    loop.SetAsSignalHandleEventLoop();

    server.set_event_loop_nums(12);
    server.set_callback_connection(std::bind(&TestTCPConnection::OnConnection, this, std::placeholders::_1));
    server.set_callback_disconnection(std::bind(&TestTCPConnection::OnDisconnection, this, std::placeholders::_1));
    server.set_callback_read(std::bind(&TestTCPConnection::OnRead, this, std::placeholders::_1, std::placeholders::_2));
    server.set_callback_write_complete(std::bind(&TestTCPConnection::OnWriteComplete, this, std::placeholders::_1));
    server.set_callback_high_water_mark(std::bind(&TestTCPConnection::OnWriteWirteHighWater, this, std::placeholders::_1, std::placeholders::_2), 100);

    server.Start();
    loop.Loop();
    server.Stop();

    return true;
}
//---------------------------------------------------------------------------
bool TestTCPConnection::Test_MultiThread()
{
    return true;
}
//---------------------------------------------------------------------------
void TestTCPConnection::OnConnection(const TCPConnPtr& conn_ptr)
{
    ConnectionAdd(conn_ptr);
    //std::cout << "OnConnectio name:" << conn_ptr->name()
    //    << " local addr:" << conn_ptr->local_addr().IPPort()
    //    << " peer addr:" << conn_ptr->peer_addr().IPPort()
    //    << std::endl;

    return;
}
//---------------------------------------------------------------------------
void TestTCPConnection::OnDisconnection(const TCPConnPtr& conn_ptr)
{
    ConnectionDel(conn_ptr);
    //std::cout << "OnDisconnectio name:" << conn_ptr->name()
    //    << " local addr:" << conn_ptr->local_addr().IPPort()
    //    << " peer addr:" << conn_ptr->peer_addr().IPPort()
    //    << std::endl;

    return;
}
//---------------------------------------------------------------------------
void TestTCPConnection::OnRead(const TCPConnPtr& conn_ptr, Buffer& rbuf)
{
//    std::cout << "read name:" << conn_ptr->name()
//        << " local addr:" << conn_ptr->local_addr().IPPort()
//        << " peer addr:" << conn_ptr->peer_addr().IPPort()
//        << std::endl;
//
//    std::cout << "rbuf data:" << rbuf.Peek() << std::endl;;

    int len = Decoder::Decode(rbuf);
    if(0 == len)
        return;

    if(rand()%50 == 1)
    {
        Close();
    }
        
    Decoder::Header header = Decoder::MakeHeader(len, Decoder::Header::kReply);
    conn_ptr->Send(reinterpret_cast<char*>(&header), sizeof(Decoder::Header));
    std::cout << "==================>>req len:" << len << "rbuf len:" << rbuf.ReadableBytes() << " char:" << rbuf.Peek()[0] << std::endl;
    conn_ptr->Send(rbuf.Peek(), len);
    rbuf.Retrieve(len);

    Notify();

    return;
}
//---------------------------------------------------------------------------
void TestTCPConnection::OnWriteComplete(const TCPConnPtr& conn_ptr)
{
    std::cout << "conn write complete:" << conn_ptr->name() << std::endl;
    return;
}
//---------------------------------------------------------------------------
void TestTCPConnection::OnWriteWirteHighWater(const TCPConnPtr& conn_ptr, size_t size)
{
    std::cout << "conn write high water:" << conn_ptr->name() << " size:" << size << std::endl;
    return;
}
//---------------------------------------------------------------------------
void TestTCPConnection::ConnectionAdd(const TCPConnPtr& conn_ptr)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto pair = tcp_connection_set_.insert(conn_ptr);
    if(false == pair.second)
    {
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
void TestTCPConnection::ConnectionDel(const TCPConnPtr& conn_ptr)
{
    std::lock_guard<std::mutex> lock(mutex_);

    tcp_connection_set_.erase(conn_ptr);
    return;
}
//---------------------------------------------------------------------------
size_t TestTCPConnection::ConnectionNums()
{
    return tcp_connection_set_.size();
}
//---------------------------------------------------------------------------
void TestTCPConnection::OnConnectionRandomDel(const TCPConnPtr& conn_ptr)
{
    TCPConnPtr ptr;

    {
    std::lock_guard<std::mutex> lock(mutex_);
    if(tcp_connection_set_.empty())
        return;
    auto iter   = tcp_connection_set_.begin();
    ptr         = *iter;
    tcp_connection_set_.erase(iter);
    }

    if(conn_ptr == ptr)
        return;

    ptr->ForceClose();
}
//---------------------------------------------------------------------------
void TestTCPConnection::Notify()
{
    TCPConnPtr ptr;
    {
    std::lock_guard<std::mutex> lock(mutex_);
    ssize_t nums = tcp_connection_set_.size();
    if(0 == nums)
        return;

    ssize_t index= rand()%nums;
    for(auto iter=tcp_connection_set_.begin(); 0<=index; ++iter)
    {
        ptr = *iter;
        index--;
    }
    } 

    std::string ip_port = ptr->peer_addr().IPPort();
    Decoder::Header header = Decoder::MakeHeader(static_cast<int>(ip_port.length()), Decoder::Header::kNotify);
    ptr->Send(reinterpret_cast<char*>(&header), sizeof(Decoder::Header));
    ptr->Send(ip_port.data(), ip_port.length());

    return;
}
//---------------------------------------------------------------------------
void TestTCPConnection::Close()
{
    TCPConnPtr ptr;
    {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t nums = tcp_connection_set_.size();
    if(0 == nums)
        return;

    size_t  index= rand()%nums;
    auto    iter = tcp_connection_set_.begin();
    for(size_t i=0; i<index; i++)
    {
        ++iter;
    }
    ptr = *iter;
    tcp_connection_set_.erase(iter);
    } 
    
    ptr->ForceClose();
    return;
}
//---------------------------------------------------------------------------
