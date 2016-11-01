//---------------------------------------------------------------------------
#include "tcp_client.h"
#include "connector.h"
#include "net_log.h"
#include "tcp_connection.h"
#include "event_loop.h"
#include "socket.h"
#include "../depend/base/include/function.h"
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
TCPClient::TCPClient(EventLoop* event_loop, const InetAddress& svr, const std::string& cli_name)
:   loop_(event_loop),
    retry_(false),
    connect_(false),
    next_conn_id_(1),
    name_(cli_name),
    connector_(new Connector(loop_, svr))
{
    NetLogger_info("TCPClient ctor");

    connector_->set_callbakc_new_connection_(std::bind(&TCPClient::NewConnection, this, std::placeholders::_1));
    return;
}
//---------------------------------------------------------------------------
TCPClient::~TCPClient()
{
    NetLogger_info("TCPClient dtor");

    //析构的时候,connection必须是只有一个引用或者没有
    if(connection_)
    {
        assert(loop_ == connection_->owner_loop());
        connection_->ForceClose();

        {
        std::lock_guard<std::mutex> lock(mutex_);
        assert(connection_.unique());
        }
    }

    connector_->Stop();
    return;
}
//---------------------------------------------------------------------------
void TCPClient::Connect()
{
    NetLogger_info("TCPClient:[%s] connect to %s", name_.c_str(), connector_->svr_addr().IPPort().c_str());

    connect_ = true;
    connector_->Start();

    return;
}
//---------------------------------------------------------------------------
void TCPClient::Disconnect()
{
    connect_ = false;
    {
    std::lock_guard<std::mutex> lock(mutex_);
    if(connection_)
        connection_->ForceClose();
    }

    return;
}
//---------------------------------------------------------------------------
void TCPClient::Stop()
{
    connect_ = false;
    connector_->Stop();

    return;
}
//---------------------------------------------------------------------------
void TCPClient::NewConnection(short sockfd)
{
    loop_->AssertInLoopThread();

    InetAddress peer_addr = Socket::GetPeerAddress(sockfd);
    std::string conn_name = base::CombineString("%s:%s#%d", name_.c_str(), peer_addr.IPPort().c_str(), next_conn_id_++);
    InetAddress local_addr(sockfd);
    TCPConnPtr conn_ptr(new TCPConn(loop_, conn_name, sockfd, local_addr, peer_addr));
    conn_ptr->set_callback_connection(callback_connection_);
    conn_ptr->set_callback_read(callback_read_);
    conn_ptr->set_callback_write_complete(callback_write_complete_);
    conn_ptr->set_callback_remove(std::bind(&TCPClient::RemoveConnection, this, std::placeholders::_1));
    conn_ptr->Initialize();

    {
    std::lock_guard<std::mutex> lock(mutex_);
    connection_ = conn_ptr;
    }

    conn_ptr->ConnectionEstablished();
    return;
}
//---------------------------------------------------------------------------
void TCPClient::RemoveConnection(const TCPConnPtr& conn_ptr)
{
    loop_->AssertInLoopThread();
    assert(loop_ == conn_ptr->owner_loop());

    {
    std::lock_guard<std::mutex> lock(mutex_);
    assert(connection_.get() == conn_ptr.get());
    connection_.reset();
    }

    loop_->QueueInLoop(std::bind(&TCPConn::ConnectionDestroy, conn_ptr));

    if(retry_ && connect_)
    {
        NetLogger_info("connect[%s] reconnect to %s", name_.c_str(), connector_->svr_addr().IPPort().c_str());
        connector_->Restart();
    }

    return;
}
//---------------------------------------------------------------------------

}//namespace net
