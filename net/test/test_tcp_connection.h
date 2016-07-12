//---------------------------------------------------------------------------
#ifndef LINUX_NET_TEST_TCP_CONNECTION_H_
#define LINUX_NET_TEST_TCP_CONNECTION_H_
//---------------------------------------------------------------------------
#include "test_base.h"
#include "../callback.h"
#include "../buffer.h"
//---------------------------------------------------------------------------
namespace net
{

namespace test
{

class TestTCPConnection : public TestBase
{
public:
    TestTCPConnection()
    {
    }
    virtual ~TestTCPConnection()
    {
    }

    virtual bool DoTest();

public:
    bool Test_Illegal();
    bool Test_Normal();
    bool Test_MultiThread();

private:
    void OnConnection   (const TCPConnPtr& conn_ptr);
    void OnDisconnection(const TCPConnPtr& conn_ptr);
    void OnRead         (const TCPConnPtr& conn_ptr, Buffer& rbuffer);
    void OnWriteComplete(const TCPConnPtr& conn_ptr);
    void OnWriteWirteHighWater(const TCPConnPtr& conn_ptr, size_t size);

private:
    void    ConnectionAdd           (const TCPConnPtr& conn_ptr);
    void    ConnectionDel           (const TCPConnPtr& conn_ptr);
    size_t  ConnectionNums          ();
    void    OnConnectionRandomDel   (const TCPConnPtr& conn_ptr);

    void    Notify();
    void    Close();

private:
    std::set<TCPConnPtr>  tcp_connection_set_;
    std::mutex                  mutex_;
};

}

}//namespace net
//---------------------------------------------------------------------------
#endif //LINUX_NET_TEST_TCP_CONNECTION_H_

