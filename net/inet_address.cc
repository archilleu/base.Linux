//---------------------------------------------------------------------------
#include "inet_address.h"
#include "../base/function.h"
#include "net_log.h"
#include <arpa/inet.h>
#include <netdb.h>
//---------------------------------------------------------------------------
namespace net
{
//---------------------------------------------------------------------------
const InetAddress InetAddress::INVALID_ADDR = InetAddress();
//---------------------------------------------------------------------------
InetAddress::InetAddress()
{
    bzero(&address_, sizeof(address_));
    return;
}
//---------------------------------------------------------------------------
InetAddress::InetAddress(short port, bool only_loopback)
{
    bzero(&address_, sizeof(address_));
    auto ip = only_loopback ? INADDR_LOOPBACK : INADDR_ANY;
    address_.sin_family     = AF_INET;
    address_.sin_port       = htobe16(port);
    address_.sin_addr.s_addr= htobe32(ip);

    return;
}
//---------------------------------------------------------------------------
InetAddress::InetAddress(const sockaddr_in& addr)
{
    address_ = addr;
    return;
}
//---------------------------------------------------------------------------
InetAddress::InetAddress(uint32_t raw_ip, short port)
{
    bzero(&address_, sizeof(address_));
    address_.sin_family     = AF_INET;
    address_.sin_port       = htobe16(port);
    address_.sin_addr.s_addr= raw_ip;

    return;
}
//---------------------------------------------------------------------------
InetAddress::InetAddress(const std::string& ip, short port)
{
    bzero(&address_, sizeof(address_));
    address_.sin_family = AF_INET;
    address_.sin_port   = htobe16(port);

    int error = ::inet_pton(AF_INET, ip.c_str(), &address_.sin_addr);
    if(0 > error) 
    {
        *this = InetAddress::INVALID_ADDR;
    }

    return;
}
//---------------------------------------------------------------------------
std::vector<InetAddress> InetAddress::GetAllByDomain(std::string domain_name, short port)
{
    std::vector<InetAddress> addr_list;

    if(domain_name.empty())
    {
        char host_name[256];
        if(0 != gethostname(host_name, sizeof(host_name)))
        return addr_list;

        domain_name = host_name;
    }

    struct addrinfo     hints;
    struct addrinfo*    result;
    bzero(&hints, sizeof(hints));
    hints.ai_family     = AF_INET;
    hints.ai_protocol   = SOCK_DGRAM;
    int err_code = getaddrinfo(domain_name.c_str(), 0, &hints, &result);
    if(0 != err_code)
    {
        SystemLog_Error("get addrinfo error:%s", gai_strerror(err_code));
        return addr_list;
    }

    for(struct addrinfo* rp=result; NULL!=rp; rp=rp->ai_next)
    {
        struct sockaddr_in* addr = reinterpret_cast<struct sockaddr_in*>(rp->ai_addr);
        addr_list.push_back(InetAddress(addr->sin_addr.s_addr, htobe16(port)));
    }

    freeaddrinfo(result);

    return addr_list;
}
//---------------------------------------------------------------------------
std::string InetAddress::IP() const
{
  char buf[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &address_.sin_addr, buf, sizeof(buf));
  return buf;
}
//---------------------------------------------------------------------------
std::string InetAddress::Port() const
{
  char buf[8];
  snprintf(buf, 8, "%u", be16toh(address_.sin_port));
  return buf;
}
//---------------------------------------------------------------------------
std::string InetAddress::IPPort() const
{
  return IP() + ":" + Port();
}
//---------------------------------------------------------------------------
}//namespace net
