//---------------------------------------------------------------------------
#ifndef LINUX_NET_TEST_CONNECTOR_H_
#define LINUX_NET_TEST_CONNECTOR_H_
//---------------------------------------------------------------------------
#include "test_base.h"
#include "../src/callback.h"
//---------------------------------------------------------------------------
namespace net
{

namespace test
{

class TestConnector: public TestBase
{
public:
    TestConnector()
    {
    }
    virtual ~TestConnector()
    {
    }

    virtual bool DoTest();

private:
    void NewConnection(int sockfd);

private:
    bool Test_Normal();
};

}//namespace test;

}//namespace net
//---------------------------------------------------------------------------
#endif //LINUX_NET_TEST_CONNECTOR_H_
