//---------------------------------------------------------------------------
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "inet_address.h"
//---------------------------------------------------------------------------
namespace net
{
//---------------------------------------------------------------------------
const InetAddress InetAddress::INVALID_ADDR = InetAddress();
//---------------------------------------------------------------------------
InetAddress::InetAddress()
{
    bzero(&address_, sizeof(address_));
}
//---------------------------------------------------------------------------
InetAddress::InetAddress(short port, bool is_ipv4, bool only_loopback)
{
    bzero(&address_, sizeof(address_));

    if(true == is_ipv4)
    {
        auto ip = only_loopback ? INADDR_LOOPBACK : INADDR_ANY;
        sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(&address_);
        addr->sin_family = AF_INET;
        addr->sin_port = htobe16(port);
        addr->sin_addr.s_addr = htobe32(ip);
    }
    else
    {
        auto ip = only_loopback ? in6addr_loopback : in6addr_any;
        sockaddr_in6* addr = reinterpret_cast<sockaddr_in6*>(&address_);
        addr->sin6_family = AF_INET6;
        addr->sin6_port = htobe16(port);
        addr->sin6_addr = ip;
    }

    return;
}
//---------------------------------------------------------------------------
InetAddress::InetAddress(const sockaddr_storage& addr)
{
    address_ = addr;
    return;
}
//---------------------------------------------------------------------------
InetAddress::InetAddress(struct in_addr raw_ip, short port)
{
    bzero(&address_, sizeof(address_));
    sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(&address_);
    addr->sin_family = AF_INET;
    addr->sin_port = htobe16(port);
    addr->sin_addr = raw_ip;

    return;
}
//---------------------------------------------------------------------------
InetAddress::InetAddress(struct in6_addr raw_ip, short port)
{
    bzero(&address_, sizeof(address_));
    sockaddr_in6* addr = reinterpret_cast<sockaddr_in6*>(&address_);
    addr->sin6_family = AF_INET6;
    addr->sin6_port = htobe16(port);
    addr->sin6_addr = raw_ip;

    return;
}
//---------------------------------------------------------------------------
InetAddress::InetAddress(const std::string& ip, short port)
{
    bzero(&address_, sizeof(address_));

    if(std::string::npos == ip.find(':'))   //ipv4
    {
        sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(&address_);
        addr->sin_family = AF_INET;
        addr->sin_port = htobe16(port);

        int error = ::inet_pton(AF_INET, ip.c_str(), &(addr->sin_addr));
        if(0 > error) 
            *this = InetAddress::INVALID_ADDR;
    }
    else    //ipv6
    {
        sockaddr_in6* addr = reinterpret_cast<sockaddr_in6*>(&address_);
        addr->sin6_family = AF_INET6;
        addr->sin6_port = htobe16(port);

        int error = ::inet_pton(AF_INET6, ip.c_str(), &(addr->sin6_addr));
        if(0 > error)
            *this = InetAddress::INVALID_ADDR;
    }

    return;
}
//---------------------------------------------------------------------------
std::string InetAddress::Ip() const
{
    if(AF_INET == address_.ss_family)
    {
        char buf[INET_ADDRSTRLEN];
        const sockaddr_in* addr = reinterpret_cast<const sockaddr_in*>(&address_);
        inet_ntop(AF_INET, &(addr->sin_addr), buf, sizeof(buf));
        return buf;
    }
    else
    {
        char buf[INET6_ADDRSTRLEN];
        const sockaddr_in6* addr = reinterpret_cast<const sockaddr_in6*>(&address_);
        inet_ntop(AF_INET6, &(addr->sin6_addr), buf, sizeof(buf));
        return buf;
    }
}
//---------------------------------------------------------------------------
std::string InetAddress::Port() const
{
    char buf[8];
    if(AF_INET == address_.ss_family)
    {
        const sockaddr_in* addr = reinterpret_cast<const sockaddr_in*>(&address_);
        snprintf(buf, 8, "%u", be16toh(addr->sin_port));
    }
    else
    {
        const sockaddr_in6* addr = reinterpret_cast<const sockaddr_in6*>(&address_);
        snprintf(buf, 8, "%u", be16toh(addr->sin6_port));
    }

    return buf;
}
//---------------------------------------------------------------------------
std::string InetAddress::IpPort() const
{
    return Ip() + ":" + Port();
}
//---------------------------------------------------------------------------
std::vector<InetAddress> InetAddress::GetList(std::string domain_name, short port)
{
    std::vector<InetAddress> addr_list;

    if(domain_name.empty())
    {
        char host_name[256];
        if(0 != gethostname(host_name, sizeof(host_name)))
            return addr_list;

        domain_name = host_name;
    }

    struct addrinfo hints;
    struct addrinfo* info;
    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_protocol = SOCK_STREAM;
    int err_code = getaddrinfo(domain_name.c_str(), 0, &hints, &info);
    if(0 != err_code)
    {
        return addr_list;
    }

    for(struct addrinfo* rp=info; NULL!=rp; rp=rp->ai_next)
    {
        if(info->ai_family == AF_INET)  //ipv4
        {
            struct sockaddr_in* addr = reinterpret_cast<struct sockaddr_in*>(rp->ai_addr);
            addr_list.push_back(InetAddress(addr->sin_addr, port));
        }
        else    //ipv6
        {
            struct sockaddr_in6* addr = reinterpret_cast<struct sockaddr_in6*>(rp->ai_addr);
            addr_list.push_back(InetAddress(addr->sin6_addr, port));
        }
    }
    freeaddrinfo(info);

    return addr_list;
}
//---------------------------------------------------------------------------

}//namespace core
//---------------------------------------------------------------------------
