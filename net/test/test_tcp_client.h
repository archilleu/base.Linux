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
    void OnConnection   (const TCPConnPtr& conn_ptr);
    void OnRead         (const TCPConnPtr& conn_ptr, Buffer& buffer, base::Timestamp rcv_time);
    void OnWriteComplete(const TCPConnPtr& conn_ptr);

private:
    bool Test_Normal();
};

}//namespace test;

}//namespace net
//---------------------------------------------------------------------------
#endif //LINUX_NET_TEST_TCP_CLIENT_H_
