//---------------------------------------------------------------------------
#include "test_tcp_server.h"
#include "../tcp_server.h"
#include "../event_loop.h"
#include "../tcp_connection.h"
#include "../buffer.h"
#include "../callback.h"
#include "../inet_address.h"
#include "../../base/thread.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
bool TestTCPServer::DoTest()
{
    if(false == Test_Illegal())     return false;
    if(false == Test_Normal())      return false;
    if(false == Test_MultiThread()) return false;

    return true;
}
//---------------------------------------------------------------------------
void TestTCPServer::OnConnection(const TCPConnectionPtr& conn_ptr)
{
    std::cout << "TestTCPServer" << ": connect:" << conn_ptr->name() << std::endl;
}
//---------------------------------------------------------------------------
void TestTCPServer::OnDisconnection(const TCPConnectionPtr& conn_ptr)
{
    std::cout << "TestTCPServer" << ": disconnect:" << conn_ptr->name() << std::endl;
}
//---------------------------------------------------------------------------
void TestTCPServer::OnThreadClientConnect()
{
    return;
}
//---------------------------------------------------------------------------
bool TestTCPServer::Test_Illegal()
{
    return true;
}
//---------------------------------------------------------------------------
static EventLoop* g_loop = 0;
//---------------------------------------------------------------------------
void Test_Normal_Finishe()
{
    g_loop->Quit();
    g_loop = 0;

    return;
}
//---------------------------------------------------------------------------
bool TestTCPServer::Test_Normal()
{
    {
    EventLoop loop;
    InetAddress listen_addr("127.0.0.1",  9999);
    TCPServer tcp_server(&loop, listen_addr);
    }

    {
    EventLoop loop;
    InetAddress listen_addr("127.0.0.1",  9999);
    TCPServer tcp_server(&loop, listen_addr);
    
    tcp_server.set_callback_connection(std::bind(&TestTCPServer::OnConnection, this, std::placeholders::_1));
    tcp_server.set_callback_disconnection(std::bind(&TestTCPServer::OnDisconnection, this, std::placeholders::_1));
    tcp_server.Start();
    
    g_loop = &loop;
    //loop.RunAfter(100, Test_Normal_Finishe);

    loop.Loop();
    tcp_server.Stop();
    }

    return true;
}
//---------------------------------------------------------------------------
bool TestTCPServer::Test_MultiThread()
{
    return true;
}
//---------------------------------------------------------------------------
