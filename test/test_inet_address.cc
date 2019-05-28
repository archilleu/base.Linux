//---------------------------------------------------------------------------
#include "test_inc.h"
#include <arpa/inet.h>
#include "../src/inet_address.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
bool Test_Normal()
{
    const char* ip6 = "fe80::20c:29ff:fead:fb6d";
    const char* ip6port = "fe80::20c:29ff:fead:fb6d:9527";

    {
    InetAddress none;
    printf("none ip:%s ip:port:%s\n", none.Ip().c_str(), none.IpPort().c_str());
    TEST_ASSERT(InetAddress::INVALID_ADDR == none);
    }

    {
    //ipv4
    InetAddress port(10000, true);
    printf("port ip:%s ip:port:%s\n", port.Ip().c_str(), port.IpPort().c_str());
    TEST_ASSERT("10000" == port.Port());

    //ipv6
    InetAddress port6(10000, false);
    printf("port ip:%s ip:port:%s\n", port6.Ip().c_str(), port6.IpPort().c_str());
    TEST_ASSERT("10000" == port6.Port());
    }

    {
    //ipv4
    struct sockaddr_storage ss;
    struct sockaddr_in* sock_in = reinterpret_cast<sockaddr_in*>(&ss);
    sock_in->sin_family = AF_INET;
    sock_in->sin_port = htobe16(9527);
    inet_pton(AF_INET, "192.168.0.1", &(sock_in->sin_addr.s_addr));
    InetAddress normal(ss);
    
    printf("normal ip:%s ip:port:%s\n", normal.Ip().c_str(), normal.IpPort().c_str());
    TEST_ASSERT("192.168.0.1:9527" == normal.IpPort());
    TEST_ASSERT(true == normal.IsV4());

    //ipv6
    struct sockaddr_in6* sock_in6 = reinterpret_cast<sockaddr_in6*>(&ss);
    sock_in->sin_family = AF_INET6;
    sock_in6->sin6_port = htobe16(9527);
    inet_pton(AF_INET6, ip6, &(sock_in6->sin6_addr));
    InetAddress normal6(ss);
    
    printf("normal6 ip:%s ip:port:%s\n", normal6.Ip().c_str(), normal6.IpPort().c_str());
    TEST_ASSERT(ip6port == normal6.IpPort());
    TEST_ASSERT(false == normal6.IsV4());
    }

    {
    //ipv4
    struct in_addr addr;
    inet_pton(AF_INET, "192.168.0.1", &addr);
    InetAddress raw(addr, 9527);
    printf("normal ip:%s ip:port:%s\n", raw.Ip().c_str(), raw.IpPort().c_str());
    TEST_ASSERT("192.168.0.1:9527" == raw.IpPort());

    //ipv6
    struct in6_addr addr6;
    inet_pton(AF_INET6, ip6, &addr6);
    InetAddress raw6(addr6, 9527);
    printf("normal ip:%s ip:port:%s\n", raw6.Ip().c_str(), raw6.IpPort().c_str());
    TEST_ASSERT(ip6port == raw6.IpPort());
    }

    {
    //ipv4
    InetAddress str("192.168.0.1", 9527);
    printf("normal ip:%s ip:port:%s\n", str.Ip().c_str(), str.IpPort().c_str());
    TEST_ASSERT("192.168.0.1:9527" == str.IpPort());

    //ipv6
    InetAddress str6(ip6, 9527);
    printf("normal ip:%s ip:port:%s\n", str6.Ip().c_str(), str6.IpPort().c_str());
    TEST_ASSERT(ip6port == str6.IpPort());
    }

    {
    const char* ip1 = "112.80.248.74";
    const char* ip2 = "112.80.248.73";

    std::vector<InetAddress> list = InetAddress::GetList("www.baidu.com", 9527);
    int count = 0;
    for(auto iter=list.begin(); iter!=list.end(); ++iter)
    {
        printf("list: %s\n", (*iter).IpPort().c_str());

        if(0 == strcmp(ip1, (*iter).Ip().c_str()))
            count++;

        if(0 == strcmp(ip2, (*iter).Ip().c_str()))
            count++;
    }

    TEST_ASSERT(0 <= count);
    }

    return true;
}
//---------------------------------------------------------------------------
int main()
{
    TestTitle();

    TEST_ASSERT(Test_Normal());

    return 0;
}
//---------------------------------------------------------------------------
