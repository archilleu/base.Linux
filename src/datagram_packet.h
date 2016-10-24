//---------------------------------------------------------------------------
#ifndef NET_DATAGRAM_PACKET_H_
#define NET_DATAGRAM_PACKET_H_
//---------------------------------------------------------------------------
#include <cassert>
#include <vector>
#include "inet_address.h"
#include "callback.h"
//---------------------------------------------------------------------------
namespace net
{

class DatagramPacket
{
public:
    DatagramPacket(size_t len=0)
    :   dat_(len),
        effective_(0)
    {
        return;
    }

    DatagramPacket(const MemoryBlock& data)
    :   dat_(data),
        effective_(0)
    {
        return;
    }

    DatagramPacket(const MemoryBlock&& data)
    :   dat_(std::move(data)),
        effective_(0)
    {
        return;
    }

    DatagramPacket(size_t len, const InetAddress& addr)
    :   dat_(len),
        effective_(0),
        address_(addr)
    {
        return;
    }

    DatagramPacket(const DatagramPacket& other)
    :   dat_(other.dat_),
        effective_(other.effective_),
        address_(other.address_)
    {
        return;
    }

    DatagramPacket& operator=(const DatagramPacket& other)
    {
        if(this == &other)
            return *this;

        dat_        = other.dat_;
        effective_  = other.effective_;
        address_    = other.address_;

        return *this;
    }

    DatagramPacket(const DatagramPacket&& other)
    :   dat_(std::move(other.dat_)),
        effective_(other.effective_),
        address_(std::move(other.address_))
    {
        return;
    }

    DatagramPacket& operator=(const DatagramPacket&& other)
    {
        if(this == &other)
            return *this;

        dat_        = std::move(other.dat_);
        effective_  = other.effective_;
        address_    = std::move(other.address_);

        return *this;
    }

    const MemoryBlock&    dat()   const   { return dat_; }
    MemoryBlock&          dat()           { return dat_; }

    const InetAddress&  address     ()                          const   { return address_; }
    void                set_address (const InetAddress& addr)           { address_ = addr; }

    size_t  effective       ()  const       { return effective_; }
    void    set_effective   (size_t len)    { assert(len <= dat_.size()); effective_ = len; }

private:
    MemoryBlock dat_;
    size_t      effective_;
    InetAddress address_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //NET_DATAGRAM_PACKET_H_
