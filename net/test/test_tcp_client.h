//---------------------------------------------------------------------------
#ifndef LINUX_NET_TEST_TCP_CLIENT_H_
#define LINUX_NET_TEST_TCP_CLIENT_H_
//---------------------------------------------------------------------------
#include "test_base.h"
#include "../callback.h"
//---------------------------------------------------------------------------
namespace net
{

namespace test
{

class TestTCPClient : public TestBase
{
public:
    TestTCPClient()
    {
    }
    virtual ~TestTCPClient()
    {
    }

    virtual bool DoTest();

private:
    void OnConnection   (const TCPConnectionPtr& conn);
    void OnRead         (const TCPConnectionPtr& conn, Buffer& buffer, base::Timestamp rcv_time);
    void OnWriteComplete(const TCPConnectionPtr& conn);

private:
    bool Test_Normal();
};

}//namespace test;

}//namespace net
//---------------------------------------------------------------------------
#endif //LINUX_NET_TEST_TCP_CLIENT_H_
