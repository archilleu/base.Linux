//---------------------------------------------------------------------------
#include "test_tcp_connection.h"
#include "../event_loop.h"
#include "../event_loop_thread_pool.h"
#include "../tcp_server.h"
#include "../tcp_connection.h"
#include "../acceptor.h"
#include "../buffer.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
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
    EventLoop   loop;
    InetAddress listen_addr("127.0.0.1", 9999);
    TCPServer   server(&loop, listen_addr);

    server.set_callback_connection(std::bind(&TestTCPConnection::OnConnection, this, std::placeholders::_1));
    server.set_callback_disconnection(std::bind(&TestTCPConnection::OnDisconnection, this, std::placeholders::_1));
    server.set_callback_read(std::bind(&TestTCPConnection::OnRead, this, std::placeholders::_1, std::placeholders::_2));

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
void TestTCPConnection::OnConnection(const TCPConnectionPtr& conn_ptr)
{
    std::cout << "OnConnectio name:" << conn_ptr->name()
        << " local addr:" << conn_ptr->local_addr().IPPort()
        << " peer addr:" << conn_ptr->peer_addr().IPPort()
        << std::endl;

    return;
}
//---------------------------------------------------------------------------
void TestTCPConnection::OnDisconnection(const TCPConnectionPtr& conn_ptr)
{
    std::cout << "OnDisconnectio name:" << conn_ptr->name()
        << " local addr:" << conn_ptr->local_addr().IPPort()
        << " peer addr:" << conn_ptr->peer_addr().IPPort()
        << std::endl;

    return;
}
//---------------------------------------------------------------------------
void TestTCPConnection::OnRead(const TCPConnectionPtr& conn_ptr, Buffer& rbuf)
{
    std::cout << "read name:" << conn_ptr->name()
        << " local addr:" << conn_ptr->local_addr().IPPort()
        << " peer addr:" << conn_ptr->peer_addr().IPPort()
        << std::endl;

    std::cout << "rbuf len:" << rbuf.ReadableBytes() << std::endl;
    std::cout << "rbuf data:" << rbuf.Peek() << std::endl;;

    conn_ptr->Send(rbuf.Peek(), rbuf.ReadableBytes());
    rbuf.RetrieveAll();

    return;
}
//---------------------------------------------------------------------------
