//---------------------------------------------------------------------------
#include <unistd.h>
#include "test_inc.h"
#include "../src/event_loop.h"
#include "../src/event_loop_thread_pool.h"
#include "../src/tcp_server.h"
#include "../src/tcp_connection.h"
#include "../src/acceptor.h"
#include "../src/buffer.h"
#include "../src/net_logger.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
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

        Header header = *reinterpret_cast<const Header*>(buffer.Peek());
        header.dat_len = be32toh(header.dat_len);
        header.type  = be32toh(header.type);
        if((sizeof(Header)+header.dat_len) > buffer.ReadableBytes())
            return 0;

        buffer.Retrieve(sizeof(Header));
        return header.dat_len;
    }

    static Header MakeHeader(int len, int type)
    {
        Header header;
        header.dat_len = htobe32(len);
        header.type = htobe32(type);
        return header;
    }
};
//---------------------------------------------------------------------------
EventLoop* g_loop;
std::mutex mutex;
std::set<TCPConnectionPtr> tcp_connection_set;
//---------------------------------------------------------------------------
void Quit()
{
    g_loop->Quit();
    return;
}
//---------------------------------------------------------------------------
void ConnectionAdd(const TCPConnectionPtr&);
void ConnectionDel(const TCPConnectionPtr&);
void Close();
void Notify();
//---------------------------------------------------------------------------
void OnConnection(const TCPConnectionPtr& conn_ptr)
{
    ConnectionAdd(conn_ptr);
    //std::cout << "OnConnectio name:" << conn_ptr->name()
    //    << " local addr:" << conn_ptr->local_addr().IPPort()
    //    << " peer addr:" << conn_ptr->peer_addr().IPPort()
    //    << std::endl;

    return;
}
//---------------------------------------------------------------------------
void OnDisconnection(const TCPConnectionPtr& conn_ptr)
{
    ConnectionDel(conn_ptr);
    //std::cout << "OnDisconnectio name:" << conn_ptr->name()
    //    << " local addr:" << conn_ptr->local_addr().IPPort()
    //    << " peer addr:" << conn_ptr->peer_addr().IPPort()
    //    << std::endl;

    return;
}
//---------------------------------------------------------------------------
void OnRead(const TCPConnectionPtr& conn_ptr, Buffer& rbuf)
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
void OnWriteComplete(const TCPConnectionPtr& conn_ptr)
{
    std::cout << "conn write complete:" << conn_ptr->name() << std::endl;
    return;
}
//---------------------------------------------------------------------------
void OnWriteWirteHighWater(const TCPConnectionPtr& conn_ptr, size_t size)
{
    std::cout << "conn write high water:" << conn_ptr->name() << " size:" << size << std::endl;
    return;
}
//---------------------------------------------------------------------------
void ConnectionAdd(const TCPConnectionPtr& conn_ptr)
{
    std::lock_guard<std::mutex> lock(mutex);

    auto pair = tcp_connection_set.insert(conn_ptr);
    if(false == pair.second)
    {
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
void ConnectionDel(const TCPConnectionPtr& conn_ptr)
{
    std::lock_guard<std::mutex> lock(mutex);

    tcp_connection_set.erase(conn_ptr);
    return;
}
//---------------------------------------------------------------------------
size_t ConnectionNums()
{
    return tcp_connection_set.size();
}
//---------------------------------------------------------------------------
void Notify()
{
    TCPConnectionPtr ptr;
    {
    std::lock_guard<std::mutex> lock(mutex);
    ssize_t nums = tcp_connection_set.size();
    if(0 == nums)
        return;

    ssize_t index= rand()%nums;
    for(auto iter=tcp_connection_set.begin(); 0<=index; ++iter)
    {
        ptr = *iter;
        index--;
    }
    } 

    std::string ip_port = ptr->peer_addr().IpPort();
    Decoder::Header header = Decoder::MakeHeader(static_cast<int>(ip_port.length()), Decoder::Header::kNotify);
    ptr->Send(reinterpret_cast<char*>(&header), sizeof(Decoder::Header));
    ptr->Send(ip_port.data(), ip_port.length());

    return;
}
//---------------------------------------------------------------------------
void Close()
{
    TCPConnectionPtr ptr;
    {
    std::lock_guard<std::mutex> lock(mutex);
    size_t nums = tcp_connection_set.size();
    if(0 == nums)
        return;

    size_t index = rand()%nums;
    auto iter = tcp_connection_set.begin();
    for(size_t i=0; i<index; i++)
    {
        ++iter;
    }
    ptr = *iter;
    tcp_connection_set.erase(iter);
    } 
    
    ptr->ForceClose();
    return;
}
//---------------------------------------------------------------------------
bool Test_Normal()
{
    auto logger = base::Logger::file_stdout_logger_mt("/tmp/logger");
    logger->set_level(base::Logger::TRACE);
    logger->set_flush_level(base::Logger::ERROR);
    EventLoop::SetLogger(logger);
    EventLoop loop;
    g_loop = &loop;
    TCPServer server(&loop, 9999);
    loop.set_sig_quit_cb(Quit);
    loop.SetHandleSingnal();

    server.set_event_loop_nums(12);
    server.set_connection_cb(std::bind(OnConnection, std::placeholders::_1));
    server.set_disconnection_cb(std::bind(OnDisconnection, std::placeholders::_1));
    server.set_read_cb(std::bind(OnRead, std::placeholders::_1, std::placeholders::_2));
    server.set_write_complete_cb(std::bind(OnWriteComplete, std::placeholders::_1));
    server.set_high_water_mark_cb(std::bind(OnWriteWirteHighWater, std::placeholders::_1, std::placeholders::_2), 100);

    server.Start();
    loop.Loop();
    server.Stop();

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
