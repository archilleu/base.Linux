//---------------------------------------------------------------------------
#include "test_acceptor.h"
#include "../acceptor.h"
#include "../event_loop.h"
#include "../event_loop_thread.h"
#include "../inet_address.h"
#include "../../base/thread.h"
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
bool TestAcceptor::Test_Normal()
{
    EventLoop       loop;
    InetAddress     listen_addr("127.0.0.1", 9999);
    Acceptor        acceptor(&loop, listen_addr);
    acceptor.Listen();

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
        return;
    }

    ::close(clientfd);
    g_loop->Quit();
    return;
}
//---------------------------------------------------------------------------
