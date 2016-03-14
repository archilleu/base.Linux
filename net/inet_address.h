//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_INET_ADDRESS_H_
#define BASE_LINUX_NET_INET_ADDRESS_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
#include <netinet/in.h>
//---------------------------------------------------------------------------
namespace net 
{

class InetAddress
{
public:
  InetAddress();
  InetAddress(short port);
  InetAddress(const struct sockaddr_in& address);
  InetAddress(uint32_t raw_ip, short port);
  InetAddress(const std::string& ip, short port);

  static std::vector<InetAddress> GetAllByName(std::string domain_name="", short port=0);//获取域名的所有ip,会进行DNS查询

public:
  std::string IP();
  std::string IPPort();

  const struct sockaddr_in& address() const { return address_; }

public:
  friend bool operator==(const InetAddress& left, const InetAddress& right);
  friend bool operator!=(const InetAddress& left, const InetAddress& right);
  friend bool operator<(const InetAddress& left, const InetAddress& right);

public:
  const static InetAddress INVALID_ADDR;

private:
  struct sockaddr_in address_;
};

inline bool operator==(const InetAddress& left, const InetAddress& right)
{
  return 0 == memcmp(&left.address_, &right.address_, sizeof(sockaddr_in));
}

inline bool operator!=(const InetAddress& left, const InetAddress& right)
{
  return !(left==right);
}

inline bool operator<(const InetAddress& left, const InetAddress& right)
{
  return (memcmp(&left.address_, &right.address_, sizeof(sockaddr_in)) < 0);
}

} //namespace net

#endif //BASE_LINUX_NET_INET_ADDRESS_H_
