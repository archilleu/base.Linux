//---------------------------------------------------------------------------
#include "test_tcp_client.h"
#include "../tcp_client.h"
#include "../event_loop.h"
#include "../tcp_connection.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
bool TestTCPClient::DoTest()
{
    if(false == Test_Normal())  return false;

    return true;
}
namespace
{
    const char* SVR_IP  = "127.0.0.1";
    const int   SVR_PORT= 9981;
};
bool flag = false;
//---------------------------------------------------------------------------
void TestTCPClient::OnConnection(const TCPConnectionPtr& conn)
{
    std::cout << "OnConnection:";
    std::cout << conn->name() << std::endl;
    flag = true;
}
//---------------------------------------------------------------------------
void TestTCPClient::OnRead(const TCPConnectionPtr& conn, Buffer& buffer, base::Timestamp rcv_time)
{
    std::cout << "time:" << rcv_time.Datetime(true) << "OnRead" << "size:" << buffer.ReadableBytes() << std::endl;
    conn->Send(buffer.Peek(), buffer.ReadableBytes());
    buffer.RetrieveAll();
    return;
}
//---------------------------------------------------------------------------
void TestTCPClient::OnWriteComplete(const TCPConnectionPtr& )
{
    std::cout << "OnWriteComplete" << std::endl;
}
//---------------------------------------------------------------------------
void SendFirst(TCPClient* client)
{
    if(!flag)
        return;

    const char* hello = "hello";
    client->connection()->Send(hello, strlen(hello));
    return;
}
//---------------------------------------------------------------------------
bool TestTCPClient::Test_Normal()
{
    EventLoop loop;
    InetAddress svr(SVR_IP, SVR_PORT);
    TCPClient client(&loop, svr, "my test");
    client.set_callback_connection(std::bind(&TestTCPClient::OnConnection, this, std::placeholders::_1));
    client.set_callback_read(std::bind(&TestTCPClient::OnRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    client.set_callback_write_complete(std::bind(&TestTCPClient::OnWriteComplete, this, std::placeholders::_1));
    client.EnableRetry();

    client.Connect();
    loop.RunAfter(5, std::bind(SendFirst, &client));
    loop.Loop();
    client.Disconnect();
    return true;
}
//---------------------------------------------------------------------------
