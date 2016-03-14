#include <WS2tcpip.h>
#include "inet_address.h"
#include "../share/function.h"

namespace aa
{

const InetAddress InetAddress::INVALID_ADDR = InetAddress();

InetAddress::InetAddress()
{
  memset(&address_, 0, sizeof(address_));
}

InetAddress::InetAddress(const sockaddr_in& address)
{
  address_ = address;
}

std::vector<InetAddress> InetAddress::GetAllByName(std::string domain_name, short port)
{
  std::vector<InetAddress> addr_list;

  if(domain_name.empty())
  {
    char host_name[256];
    if(0 != gethostname(host_name, sizeof(host_name)))
      return std::move(addr_list);

    domain_name = host_name;
  }

  struct hostent* remote_host =gethostbyname(domain_name.c_str());
  if(0 == remote_host)
    return std::move(addr_list);

  if(AF_INET == remote_host->h_addrtype)
  {
    for(int i=0; 0 !=remote_host->h_addr_list[i]; i++)
    {
      addr_list.push_back(std::move(InetAddress::GetByAddress(*(uint32_t*) remote_host->h_addr_list[i], port)));
    }
  }

  return std::move(addr_list);
}

InetAddress InetAddress::GetByName(const std::string& ip, short port)
{
  InetAddress inet_addr;
  inet_addr.address_.sin_family = AF_INET; //Address families
  inet_addr.address_.sin_port = zshare::htobe16(port);

  int error = ::inet_pton(AF_INET, ip.c_str(), &inet_addr.address_.sin_addr);
  if(1 > ::inet_pton(AF_INET, ip.c_str(), &inet_addr.address_.sin_addr))
    return InetAddress::INVALID_ADDR;

  return inet_addr;
}

InetAddress InetAddress::GetByAddress(short port)
{
  InetAddress inet_addr;
  inet_addr.address_.sin_family = AF_INET; //Address families
  inet_addr.address_.sin_port = zshare::htobe16(port);
  inet_addr.address_.sin_addr.S_un.S_addr = 0;

  return inet_addr;
}

InetAddress InetAddress::GetByAddress(uint32_t raw_ip, short port)
{
  InetAddress inet_addr;
  inet_addr.address_.sin_family = AF_INET; //Address families
  inet_addr.address_.sin_port = zshare::htobe16(port);
  inet_addr.address_.sin_addr.S_un.S_addr = raw_ip;

  return inet_addr;
}

std::string InetAddress::GetIP()
{
  char buf[16];
  inet_ntop(AF_INET, &address_.sin_addr, buf, 16);
  return buf;
}

std::string InetAddress::ToString()
{
  char buf[8];
  _itoa_s(zshare::be16toh(address_.sin_port), buf, 8, 10);

  return (GetIP() + ":" + buf);
}

}
