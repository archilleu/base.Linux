//---------------------------------------------------------------------------
#include "tcp_server.h"
#include "event_loop.h"
#include "acceptor.h"
#include "inet_address.h"
#include "socket.h"
#include "net_log.h"
#include "tcp_connection.h"
#include "event_loop_thread_pool.h"
#include "../base/function.h"
//---------------------------------------------------------------------------
namespace net
{
//---------------------------------------------------------------------------
TCPServer::TCPServer(EventLoop* owner_loop, InetAddress& listen_addr)
:   owner_loop_(owner_loop),
    name_(listen_addr.IPPort()),
    next_connect_id_(0),
    acceptor_(new Acceptor(owner_loop, listen_addr))
{
    acceptor_->set_callback_new_connection(std::bind(&TCPServer::OnNewConnection, this, 
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    return;
}
//---------------------------------------------------------------------------
TCPServer::~TCPServer()
{
    return;
}
//---------------------------------------------------------------------------
void TCPServer::set_event_loop_nums(int nums)
{
    loop_thread_pool_->set_thread_nums(nums);    
}
//---------------------------------------------------------------------------
void TCPServer::OnNewConnection(int clientfd, const InetAddress& client_addr, base::Timestamp accept_time)
{
    owner_loop_->AssertInLoopThread();

    EventLoop* loop = loop_thread_pool_->GetNextEventLoop();

    std::string new_conn_name = base::CombineString("%s#%zu", name_.c_str(), next_connect_id_++);
    InetAddress local_addr = Socket::GetLocalAddress(clientfd);

    SystemLog_Debug("accept time:%s, new connection server name:[%s], total[%zu]- from :%s to :%s\n", accept_time.Datetime(true).c_str(),
            name_.c_str(), tcp_name_connection_map_.size(), local_addr.IPPort().c_str(), client_addr.IPPort().c_str());

    TCPConnectionPtr conn_ptr = std::make_shared<TCPConnection>(loop, new_conn_name, clientfd, local_addr, client_addr);
    //todo conn_ptr
    
    tcp_name_connection_map_[new_conn_name] = conn_ptr;
    
    //连接就绪
    return;
}
//---------------------------------------------------------------------------
void TCPServer::OnConnectionDestroy(const TCPConnectionPtr& connection_ptr)
{
}
//---------------------------------------------------------------------------
void TCPServer::OnConnectionDestroyInLoop(const TCPConnectionPtr& connection_ptr)
{
}
//---------------------------------------------------------------------------
}//namespace net
