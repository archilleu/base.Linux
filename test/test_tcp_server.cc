//---------------------------------------------------------------------------
#include "test_tcp_server.h"
#include "../src/tcp_server.h"
#include "../src/event_loop.h"
#include "../src/tcp_connection.h"
#include "../src/buffer.h"
#include "../src/callback.h"
#include "../src/inet_address.h"
#include "../depend/base/include/thread.h"
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
void TestTCPServer::OnConnection(const TCPConnPtr& conn_ptr)
{
    std::cout << "TestTCPServer" << ": connect:" << conn_ptr->name() << std::endl;
    std::cout << "count:" <<  conn_ptr.use_count() << std::endl;
}
//---------------------------------------------------------------------------
void TestTCPServer::OnDisconnection(const TCPConnPtr& conn_ptr)
{
    std::cout << "TestTCPServer" << ": disconnect:" << conn_ptr->name() << std::endl;
    std::cout << "count:" <<  conn_ptr.use_count() << std::endl;
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
namespace
{

TCPServer* svr;
EventLoop* g_loop;
void Dump()
{
    svr->DumpConnection();
}

void Quit()
{
    g_loop->Quit();
}


}
//---------------------------------------------------------------------------
bool TestTCPServer::Test_Normal()
{
    {
    EventLoop loop;
    InetAddress listen_addr(9999);
    std::vector<InetAddress> addrs;
    addrs.push_back(listen_addr);
    TCPServer tcp_server(&loop, addrs);
    }

    {
    EventLoop loop;
    g_loop = &loop;
    TCPServer tcp_server(&loop, 9999);
    svr = &tcp_server;
    
    loop.set_sig_usr1_callback(Dump);
    loop.set_sig_quit_callback(Quit);
    loop.SetAsSignalHandleEventLoop();
    tcp_server.set_callback_connection(std::bind(&TestTCPServer::OnConnection, this, std::placeholders::_1));
    tcp_server.set_callback_disconnection(std::bind(&TestTCPServer::OnDisconnection, this, std::placeholders::_1));
    tcp_server.Start();
    loop.Loop();
    tcp_server.Stop();
    }

    return true;
}
//---------------------------------------------------------------------------
bool TestTCPServer::Test_MultiThread()
{
    EventLoop loop;
    g_loop = &loop;
    TCPServer tcp_server(&loop, 9999);
    loop.set_sig_usr1_callback(Dump);
    loop.set_sig_quit_callback(Quit);
    loop.SetAsSignalHandleEventLoop();
    tcp_server.set_event_loop_nums(8);
    tcp_server.set_callback_connection(std::bind(&TestTCPServer::OnConnection, this, std::placeholders::_1));
    tcp_server.set_callback_disconnection(std::bind(&TestTCPServer::OnDisconnection, this, std::placeholders::_1));
    tcp_server.Start();
    loop.Loop();
    tcp_server.Stop();

    return true;
}
//---------------------------------------------------------------------------
