//---------------------------------------------------------------------------
#include "test_inc.h"
#include "../src/socket.h"
#include "../src/inet_address.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
int fd = ::socket(AF_INET, SOCK_STREAM, 0);
//---------------------------------------------------------------------------
bool Test_Normal()
{
    Socket sock(fd);
    printf("socket fd:%d\n", sock.fd());

    int slen = sock.GetSendBufferSize();
    int rlen = sock.GetRecvBufferSize();
    printf("socket send size:%d recv size:%d\n", slen, rlen);

    slen = 1024*100;
    rlen = 1024*200;
    sock.SetSendBufferSize(slen);
    sock.SetRecvBufferSize(rlen);
    slen = sock.GetSendBufferSize();
    rlen = sock.GetRecvBufferSize();
    printf("socket send size:%d recv size:%d\n", slen, rlen);

    printf("local addr:%s perr addr:\n", sock.GetLocalAddress().IpPort().c_str()); 

    sock.Bind(InetAddress("127.0.0.1", 9999));
    printf("local addr:%s perr addr:\n", sock.GetLocalAddress().IpPort().c_str());

    sock.SetKeepAlive(60);
    int val = 0;                                                                                                                                                              
    socklen_t   len = sizeof(val);
    if(0 > getsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char*>(&val), &len))
    {
        assert(0);
        return false;
    }
    else
    {
        assert(1 == val);
    }

    std::string info = sock.GetTCPInfoString();
    std::cout << "tcp info:" << info << std::endl;
    return true;
}
//---------------------------------------------------------------------------
int main()
{
    TEST_ASSERT(Test_Normal());

    return 0;
}
//---------------------------------------------------------------------------
