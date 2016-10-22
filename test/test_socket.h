//---------------------------------------------------------------------------
#ifndef LINUX_NET_TEST_SOCKET_H_
#define LINUX_NET_TEST_SOCKET_H_
//---------------------------------------------------------------------------
#include "test_base.h"
#include <sys/socket.h>
#include <unistd.h>
//---------------------------------------------------------------------------
namespace net
{

namespace test
{

class TestSocket : public TestBase
{
public:
    TestSocket();
    virtual ~TestSocket();

    virtual bool DoTest();

private:
    bool Test_Normal();

private:
    int fd_;
};

}

}//namespace net
//---------------------------------------------------------------------------
#endif //LINUX_NET_TEST_SOCKET_H_
