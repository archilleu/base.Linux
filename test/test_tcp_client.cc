//---------------------------------------------------------------------------
#include "test_inc.h"
#include <condition_variable>
#include <queue>
#include "../src/tcp_client.h"
#include "../src/event_loop.h"
#include "../src/tcp_connection.h"
#include "../thirdpart/base/include/timestamp.h"
#include "../thirdpart/base/include/thread.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
namespace
{
    const char* SVR_IP  = "127.0.0.1";
    const int SVR_PORT= 9981;
    EventLoop* g_loop = 0;
    TCPClient* g_client = 0;

    std::mutex mutex;
    std::condition_variable cond;
    bool flag = false;

    std::queue<std::string> msg_queue;
};
//---------------------------------------------------------------------------
void OnConnection(const TCPConnectionPtr& conn_ptr)
{
    std::cout << "OnConnection:";
    std::cout << conn_ptr->name() << std::endl;

    std::cout << "count:" << conn_ptr.use_count() << std::endl;

    std::unique_lock<std::mutex> lock(mutex);
    flag = true;
    cond.notify_one();
}
//---------------------------------------------------------------------------
void OnRead(const TCPConnectionPtr&, Buffer& buffer, uint64_t rcv_time)
{
    std::cout << "time:" << base::Timestamp(rcv_time).Datetime(true) << "OnRead" << "size:" << buffer.ReadableBytes() << std::endl;
    
    const std::string& msg = msg_queue.front();
    if(msg.size() > buffer.ReadableBytes())
        return;

    std::string msg_rcv(buffer.Peek(), msg.size());
    buffer.Retrieve(msg.size());

    if(msg != msg_rcv)
        assert(0);
    msg_queue.pop();

    std::unique_lock<std::mutex> lock(mutex);
    flag = true;
    cond.notify_one();

    return;
}
//---------------------------------------------------------------------------
void OnWriteComplete(const TCPConnectionPtr& )
{
    std::cout << "OnWriteComplete" << std::endl;
}
//---------------------------------------------------------------------------
void OnThreadSend()
{
    size_t size = msg_queue.size();

    for(size_t i=0; i<size; i++)
    {
        {
        std::unique_lock<std::mutex> lock(mutex);
        while(!flag)
            cond.wait(lock);
        flag = false;
        }
        
        TCPConnectionPtr conn_ptr = g_client->connection();
        if(conn_ptr)
        {
            const std::string& msg =  msg_queue.front();
            conn_ptr->Send(msg.data(), msg.length());
        }
    }

    g_client->Disconnect();
    g_loop->Quit();
    return;
}
//---------------------------------------------------------------------------
bool Test_Normal()
{
    for(size_t i=0; i<1024*64; i++)
    {
        std::string msg(1024, static_cast<char>(rand()%256));
        msg_queue.push(std::move(msg));
    }

    EventLoop loop;
    g_loop = &loop;
    InetAddress svr(SVR_IP, SVR_PORT);
    TCPClient client(&loop, svr, "my test");
    g_client = &client;
    client.set_conn_cb(std::bind(OnConnection, std::placeholders::_1));
    client.set_read_cb(std::bind(OnRead, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    client.set_write_complete_cb(std::bind(OnWriteComplete, std::placeholders::_1));
    client.EnableRetry();

    client.Connect();
    base::Thread thread(OnThreadSend);
    thread.Start();
    loop.Loop();
    thread.Join();
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
