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
    const char* ip6 = "fe80::20c:29ff:fead:fb6d";
    const char* ip6port = "fe80::20c:29ff:fead:fb6d(9527)";

    {
    InetAddress none;
    printf("none ip:%s ip:port:%s\n", none.IP().c_str(), none.IPPort().c_str());
    MY_ASSERT(InetAddress::INVALID_ADDR == none);
    }

    {
    //ipv4
    InetAddress port(10000, true);
    printf("port ip:%s ip:port:%s\n", port.IP().c_str(), port.IPPort().c_str());
    MY_ASSERT("10000" == port.Port());

    //ipv6
    InetAddress port6(10000, false);
    printf("port ip:%s ip:port:%s\n", port6.IP().c_str(), port6.IPPort().c_str());
    MY_ASSERT("10000" == port6.Port());
    }

    {
    //ipv4
    struct sockaddr_storage ss;
    struct sockaddr_in* sock_in = reinterpret_cast<sockaddr_in*>(&ss);
    sock_in->sin_family = AF_INET;
    sock_in->sin_port = htobe16(9527);
    inet_pton(AF_INET, "192.168.0.1", &(sock_in->sin_addr.s_addr));
    InetAddress normal(ss);
    
    printf("normal ip:%s ip:port:%s\n", normal.IP().c_str(), normal.IPPort().c_str());
    MY_ASSERT("192.168.0.1:9527" == normal.IPPort());
    MY_ASSERT(true == normal.IsIPV4());

    //ipv6
    struct sockaddr_in6* sock_in6 = reinterpret_cast<sockaddr_in6*>(&ss);
    sock_in->sin_family = AF_INET6;
    sock_in6->sin6_port = htobe16(9527);
    inet_pton(AF_INET6, ip6, &(sock_in6->sin6_addr));
    InetAddress normal6(ss);
    
    printf("normal6 ip:%s ip:port:%s\n", normal6.IP().c_str(), normal6.IPPort().c_str());
    MY_ASSERT(ip6port == normal6.IPPort());
    MY_ASSERT(false == normal6.IsIPV4());
    }

    {
    //ipv4
    struct in_addr addr;
    inet_pton(AF_INET, "192.168.0.1", &addr);
    InetAddress raw(addr, 9527);
    printf("normal ip:%s ip:port:%s\n", raw.IP().c_str(), raw.IPPort().c_str());
    MY_ASSERT("192.168.0.1:9527" == raw.IPPort());

    //ipv6
    struct in6_addr addr6;
    inet_pton(AF_INET6, ip6, &addr6);
    InetAddress raw6(addr6, 9527);
    printf("normal ip:%s ip:port:%s\n", raw6.IP().c_str(), raw6.IPPort().c_str());
    MY_ASSERT(ip6port == raw6.IPPort());
    }

    {
    //ipv4
    InetAddress str("192.168.0.1", 9527);
    printf("normal ip:%s ip:port:%s\n", str.IP().c_str(), str.IPPort().c_str());
    MY_ASSERT("192.168.0.1:9527" == str.IPPort());

    //ipv6
    InetAddress str6(ip6, 9527);
    printf("normal ip:%s ip:port:%s\n", str6.IP().c_str(), str6.IPPort().c_str());
    MY_ASSERT(ip6port == str6.IPPort());
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

    MY_ASSERT(0 <= count);
    }

    return true;
}
//---------------------------------------------------------------------------
