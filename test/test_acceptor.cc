//---------------------------------------------------------------------------
#include <unistd.h>
#include "test_acceptor.h"
#include "../src/acceptor.h"
#include "../src/event_loop.h"
#include "../src/event_loop_thread.h"
#include "../src/inet_address.h"
#include "../depend/base/include/thread.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
bool TestAcceptor::DoTest()
{
    if(false == Test_Illgal())  return false;
    if(false == Test_Normal())  return false;

    return true;
}
//---------------------------------------------------------------------------
bool TestAcceptor::Test_Illgal()
{
//    EventLoop loop;
//    {
//    InetAddress listen_addr;
//    Acceptor    acceptor(0, listen_addr);
//    }

    return true;
}
//---------------------------------------------------------------------------
static EventLoop* g_loop = 0;
//---------------------------------------------------------------------------
void NewConn(int fd, const InetAddress& addr, uint64_t rcv_time)
{
    std::cout << "=====================>rcv: " << base::Timestamp(rcv_time).Datetime(true) <<  "fd:" << fd << " addr:"
        << addr.IPPort() << std::endl;
}
//---------------------------------------------------------------------------
bool TestAcceptor::Test_Normal()
{
    EventLoop       loop;
    //ipv4
    InetAddress     listen_addr(9999, true);
    InetAddress     listen_addr6(9999, false);
    Acceptor        acceptor(&loop, listen_addr);
    Acceptor        acceptor6(&loop, listen_addr6);
    acceptor.set_callback_new_connection(NewConn);
    acceptor6.set_callback_new_connection(NewConn);
    acceptor.Listen();
    acceptor6.Listen();

    g_loop = &loop;

    //启动客户端请求连接线程
    base::Thread t(std::bind(&TestAcceptor::ClientConnect, this));
    t.Start();

    loop.Loop();
    return true;
}
//---------------------------------------------------------------------------
void TestAcceptor::ClientConnect()
{
    //ipv4
    {
    int clientfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if(0 > clientfd)
    {
        printf("socket error:%s\n", strerror(errno));
        return;
    }

    InetAddress inet_svr("127.0.0.1", 9999);
    if(0 > ::connect(clientfd, reinterpret_cast<const sockaddr*>(&(inet_svr.address())), sizeof(sockaddr)))
    {
        printf("connect error:%s\n", strerror(errno));
        ::close(clientfd);
    }

    ::close(clientfd);
    }
    //ipv6
    {
    int clientfd = ::socket(AF_INET6, SOCK_STREAM, 0);
    if(0 > clientfd)
    {
        printf("socket error:%s\n", strerror(errno));
        return;
    }

    InetAddress inet_svr6(9999, false);
    if(0 > ::connect(clientfd, reinterpret_cast<const sockaddr*>(&(inet_svr6.address())), sizeof(inet_svr6.address())))
    {
        printf("connect error:%s\n", strerror(errno));
        ::close(clientfd);
        return;
    }

    ::close(clientfd);

    }
    g_loop->Quit();
    return;
}
//---------------------------------------------------------------------------
