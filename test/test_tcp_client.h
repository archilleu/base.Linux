//---------------------------------------------------------------------------
#ifndef LINUX_NET_TEST_TCP_CLIENT_H_
#define LINUX_NET_TEST_TCP_CLIENT_H_
//---------------------------------------------------------------------------
#include "test_base.h"
#include "../src/callback.h"
#include "../depend/base/include/thread.h"
//---------------------------------------------------------------------------
namespace net
{

namespace test
{

class TestTCPClient : public TestBase
{
public:
    TestTCPClient()
    :   thread_(std::bind(&TestTCPClient::OnThreadSend, this)),
        flag_(true)
    {
    }
    virtual ~TestTCPClient()
    {
    }

    virtual bool DoTest();

private:
    void OnConnection   (const TCPConnPtr& conn_ptr);
    void OnRead         (const TCPConnPtr& conn_ptr, Buffer& buffer, base::Timestamp rcv_time);
    void OnWriteComplete(const TCPConnPtr& conn_ptr);

private:
    void OnThreadSend();

    base::Thread thread_;

    std::mutex mutex_;
    std::condition_variable cond_;
    bool flag_;

    std::queue<std::string> msg_queue_;


private:
    bool Test_Normal();
};

}//namespace test;

}//namespace net
//---------------------------------------------------------------------------
#endif //LINUX_NET_TEST_TCP_CLIENT_H_
