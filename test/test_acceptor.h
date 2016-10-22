//---------------------------------------------------------------------------
#ifndef LINUX_NET_TEST_ACCEPTOR_H_
#define LINUX_NET_TEST_ACCEPTOR_H_
//---------------------------------------------------------------------------
#include "test_base.h"
//---------------------------------------------------------------------------
namespace net
{

namespace test
{

class TestAcceptor : public TestBase
{
public:
    TestAcceptor()
    {
    }

    virtual ~TestAcceptor()
    {
    }

    virtual bool DoTest();

private:
    bool Test_Illgal();
    bool Test_Normal();

    void ClientConnect();
};

}//namespace test

}//namespace net
//---------------------------------------------------------------------------
#endif//LINUX_NET_TEST_ACCEPTOR_H_
