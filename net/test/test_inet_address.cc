//---------------------------------------------------------------------------
#include "test_inet_address.h"
#include <arpa/inet.h>
#include "../inet_address.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace test;
//---------------------------------------------------------------------------
bool TestInetAddress::DoTest()
{
    if(false == Test_Normal())  return false;

    return true;
}
//---------------------------------------------------------------------------
bool TestInetAddress::Test_Normal()
{
    {
    InetAddress none;
    printf("none ip:%s ip:port:%s\n", none.IP().c_str(), none.IPPort().c_str());
    MY_ASSERT(InetAddress::INVALID_ADDR == none);
    }

    {
    InetAddress port(10000);
    printf("port ip:%s ip:port:%s\n", port.IP().c_str(), port.IPPort().c_str());
    MY_ASSERT("10000" == port.Port());
    }

    {
    struct sockaddr_in sock_in;
    sock_in.sin_port = htobe16(9527);
    inet_pton(AF_INET, "192.168.0.1", &sock_in.sin_addr.s_addr);
    InetAddress normal(sock_in);
    
    printf("normal ip:%s ip:port:%s\n", normal.IP().c_str(), normal.IPPort().c_str());
    MY_ASSERT("192.168.0.1:9527" == normal.IPPort());
    }

    {
    uint32_t addr;
    inet_pton(AF_INET, "192.168.0.1", &addr);
    InetAddress raw(addr, 9527);
    printf("normal ip:%s ip:port:%s\n", raw.IP().c_str(), raw.IPPort().c_str());
    MY_ASSERT("192.168.0.1:9527" == raw.IPPort());
    }

    {
    InetAddress str("192.168.0.1", 9527);
    printf("normal ip:%s ip:port:%s\n", str.IP().c_str(), str.IPPort().c_str());
    MY_ASSERT("192.168.0.1:9527" == str.IPPort());
    }

    {
    const char* ip1 = "112.80.248.74";
    const char* ip2 = "112.80.248.73";

    std::vector<InetAddress> list = InetAddress::GetAllByDomain("www.baidu.com", 9527);
    int count = 0;
    for(auto iter=list.begin(); iter!=list.end(); ++iter)
    {
        printf("list: %s\n", (*iter).IPPort().c_str());

        if(0 == strcmp(ip1, (*iter).IP().c_str()))
            count++;

        if(0 == strcmp(ip2, (*iter).IP().c_str()))
            count++;
    }

    MY_ASSERT(1 <= count);
    }

    return true;
}
//---------------------------------------------------------------------------
