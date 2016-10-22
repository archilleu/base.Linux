//---------------------------------------------------------------------------
#ifndef LINUX_NET_TEST_TCP_SERVER_H_
#define LINUX_NET_TEST_TCP_SERVER_H_
//---------------------------------------------------------------------------
#include "test_base.h"
#include "../src/callback.h"
//---------------------------------------------------------------------------
namespace net
{

namespace test
{

class TestTCPServer : public TestBase
{
public:
    TestTCPServer()
    {
    }
    virtual ~TestTCPServer()
    {
    }

    virtual bool DoTest();

private:
    void OnConnection   (const TCPConnPtr& conn_ptr);
    void OnDisconnection(const TCPConnPtr& conn_ptr);

    void OnThreadClientConnect();

private:
    bool Test_Illegal();
    bool Test_Normal();
    bool Test_MultiThread();
};

}//namespace test

}//namespace net
//---------------------------------------------------------------------------
#endif //LINUX_NET_TEST_CONNECTION_H_
