//---------------------------------------------------------------------------
#include "tcp_connection.h"
#include "event_loop.h"
//---------------------------------------------------------------------------
namespace net
{
//---------------------------------------------------------------------------
TCPConnection::TCPConnection(EventLoop* owner_loop, const std::string& name, int fd, const InetAddress& local_addr, const InetAddress& peer_addr)
{
}
//---------------------------------------------------------------------------
TCPConnection::~TCPConnection()
{
}
//---------------------------------------------------------------------------
}//namespace net
