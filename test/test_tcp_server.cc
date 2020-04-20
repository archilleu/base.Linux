//---------------------------------------------------------------------------
#include "test_inc.h"
#include "../src/tcp_server.h"
#include "../src/event_loop.h"
#include "../src/tcp_connection.h"
#include "../src/buffer.h"
#include "../src/callback.h"
#include "../src/inet_address.h"
#include "../thirdpart/base/include/thread.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
void OnConnection(const TCPConnectionPtr& conn_ptr)
{
    std::cout << "TestTCPServer" << ": connect:" << conn_ptr->name() << std::endl;
    std::cout << "count:" <<  conn_ptr.use_count() << std::endl;
    if(!conn_ptr->data().empty())
    {
        std::cout << "any data: " << base::any_cast<std::string>(conn_ptr->data()) << std::endl;
    }
}
//---------------------------------------------------------------------------
void OnDisconnection(const TCPConnectionPtr& conn_ptr)
{
    std::cout << "TestTCPServer" << ": disconnect:" << conn_ptr->name() << std::endl;
    std::cout << "count:" <<  conn_ptr.use_count() << std::endl;
}
//---------------------------------------------------------------------------
void OnThreadClientConnect()
{
    return;
}
//---------------------------------------------------------------------------
TCPServer* svr;
EventLoop* g_loop;
//---------------------------------------------------------------------------
void Dump()
{
    svr->DumpConnections();
}
//---------------------------------------------------------------------------
void Quit()
{
    g_loop->Quit();
}
//---------------------------------------------------------------------------
bool Test_Normal()
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
    
    loop.set_sig_usr1_cb(Dump);
    loop.set_sig_quit_cb(Quit);
    loop.SetHandleSingnal();
    tcp_server.set_connection_cb(std::bind(OnConnection, std::placeholders::_1));
    tcp_server.set_disconnection_cb(std::bind(OnDisconnection, std::placeholders::_1));
    tcp_server.Start();
    loop.Loop();
    tcp_server.Stop();
    }

    return true;
}
//---------------------------------------------------------------------------
bool Test_MultiThread()
{
    EventLoop loop;
    g_loop = &loop;
    TCPServer tcp_server(&loop, 9999);
    loop.set_sig_usr1_cb(Dump);
    loop.set_sig_quit_cb(Quit);
    loop.SetHandleSingnal();
    tcp_server.set_event_loop_nums(8);
    tcp_server.set_connection_cb(std::bind(OnConnection, std::placeholders::_1));
    tcp_server.set_disconnection_cb(std::bind(OnDisconnection, std::placeholders::_1));
    tcp_server.Start();
    loop.Loop();
    tcp_server.Stop();

    return true;
}
//---------------------------------------------------------------------------
bool Test_MultiThreadData()
{
    EventLoop loop;
    g_loop = &loop;
    InetAddress listen_addr(9999);
    TCPServer tcp_server(&loop, {{listen_addr}});
    loop.set_sig_usr1_cb(Dump);
    loop.set_sig_quit_cb(Quit);
    loop.SetHandleSingnal();
    tcp_server.set_event_loop_nums(8);
    tcp_server.set_connection_cb(std::bind(OnConnection, std::placeholders::_1));
    tcp_server.set_disconnection_cb(std::bind(OnDisconnection, std::placeholders::_1));
    tcp_server.Start();
    loop.Loop();
    tcp_server.Stop();

    return true;
}
//---------------------------------------------------------------------------
int main()
{
    TestTitle();

    //TEST_ASSERT(Test_Normal());
    //TEST_ASSERT(Test_MultiThread());
    TEST_ASSERT(Test_MultiThreadData());
    return 0;
}
//---------------------------------------------------------------------------
