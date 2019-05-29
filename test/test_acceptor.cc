//---------------------------------------------------------------------------
#include <unistd.h>
#include "test_inc.h"
#include "../src/acceptor.h"
#include "../src/socket.h"
#include "../src/event_loop.h"
#include "../src/event_loop_thread.h"
#include "../src/inet_address.h"
#include "../thirdpart/base/include/thread.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
bool Test_Illgal()
{
    EventLoop loop;
    {
    //InetAddress listen_addr;
    //Acceptor    acceptor(0, listen_addr);
    }

    return true;
}
//---------------------------------------------------------------------------
static EventLoop* g_loop = 0;
//---------------------------------------------------------------------------
void NewConn(Socket socket, const InetAddress& addr, uint64_t rcv_time)
{
    std::cout << "=====================>time: " << base::Timestamp(rcv_time).Datetime(true) <<  " fd:" << socket.fd() << " addr:"
        << addr.IpPort() << std::endl;
    sleep(1);
}
//---------------------------------------------------------------------------
void NewConnData(Socket socket, const InetAddressConfig& addr, uint64_t rcv_time)
{
    std::cout << "=====================>time: " 
        << base::Timestamp(rcv_time).Datetime(true) <<  " fd:" << socket.fd() << " addr:"
        << addr.address.IpPort()
        << " any data:" << base::any_cast<std::string>(addr.data) << std::endl;
    sleep(1);
}
//---------------------------------------------------------------------------
void ClientConnect();
//---------------------------------------------------------------------------
bool Test_Normal()
{
    EventLoop loop;
    InetAddress listen_addr(9999, true); //ipv4
    InetAddress listen_addr6(9999, false);
    Acceptor acceptor(&loop, listen_addr);
    Acceptor acceptor6(&loop, listen_addr6);
    acceptor.set_new_conn_cb(NewConn);
    acceptor6.set_new_conn_cb(NewConn);
    acceptor.Listen();
    acceptor6.Listen();

    g_loop = &loop;

    //启动客户端请求连接线程
    base::Thread t(ClientConnect);
    t.Start();

    loop.Loop();
    return true;
}
//---------------------------------------------------------------------------
bool Test_NormalData()
{
    EventLoop loop;
    base::any data = std::string("hello,xxx");
    InetAddress listen_addr(9999, true); //ipv4
    InetAddress listen_addr6(9999, false);
    Acceptor acceptor(&loop, {listen_addr, data});
    Acceptor acceptor6(&loop, {listen_addr6, data});
    acceptor.set_new_conn_data_cb(NewConnData);
    acceptor6.set_new_conn_data_cb(NewConnData);
    acceptor.Listen();
    acceptor6.Listen();

    g_loop = &loop;

    //启动客户端请求连接线程
    base::Thread t(ClientConnect);
    t.Start();

    loop.Loop();
    return true;
}
//---------------------------------------------------------------------------
void ClientConnect()
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

    sleep(2);
    g_loop->Quit();
    return;
}
//---------------------------------------------------------------------------
int main()
{
    TestTitle();

    TEST_ASSERT(Test_Illgal());
    TEST_ASSERT(Test_Normal());
    TEST_ASSERT(Test_NormalData());

    return 1;
}
//---------------------------------------------------------------------------
