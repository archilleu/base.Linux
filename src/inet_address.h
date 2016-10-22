//---------------------------------------------------------------------------
#ifndef NET_INET_ADDRESS_H_
#define NET_INET_ADDRESS_H_
//---------------------------------------------------------------------------
#include <string>
#include <vector>
#include <cstring>
#include <netinet/in.h>
//---------------------------------------------------------------------------
namespace net 
{

class InetAddress
{
public:
    InetAddress();
    explicit InetAddress(short port, bool is_ipv4=true, bool only_loopback=false);
    InetAddress(const struct sockaddr_storage& address);
    InetAddress(struct in_addr raw_ip, short port);
    InetAddress(struct in6_addr raw_ip, short port);
    InetAddress(const std::string& ip, short port); 

public:
    void set_address(const sockaddr_storage& addr)    { address_ = addr; }

    std::string IP()    const;
    std::string Port()  const;
    std::string IPPort()const;

    bool IsIPV4() const { return (AF_INET == address_.ss_family); }

    const struct sockaddr_storage& address() const { return address_; }

public:
    static std::vector<InetAddress> GetAllByDomain(std::string domain_name="", short port=0);//获取域名的所有ip,会进行DNS查询

public:
    friend bool operator==(const InetAddress& left, const InetAddress& right);
    friend bool operator!=(const InetAddress& left, const InetAddress& right);
    friend bool operator <(const InetAddress& left, const InetAddress& right);
    friend bool operator >(const InetAddress& left, const InetAddress& right);

public:
    const static InetAddress INVALID_ADDR;

private:
    struct sockaddr_storage address_;
};
//---------------------------------------------------------------------------
inline bool operator==(const InetAddress& left, const InetAddress& right)
{
    return (0 == memcmp(&left.address_, &right.address_, sizeof(sockaddr_in)));
}
//---------------------------------------------------------------------------
inline bool operator!=(const InetAddress& left, const InetAddress& right)
{
    return !(left==right);
}
//---------------------------------------------------------------------------
inline bool operator<(const InetAddress& left, const InetAddress& right)
{
    return (memcmp(&left.address_, &right.address_, sizeof(sockaddr_in)) < 0);
}
//---------------------------------------------------------------------------
inline bool operator>(const InetAddress& left, const InetAddress& right)
{
    return !(left < right);
}
//---------------------------------------------------------------------------
} //namespace net
//---------------------------------------------------------------------------
#endif //NET_INET_ADDRESS_H_
