//---------------------------------------------------------------------------
#include "tcp_client.h"
#include "connector.h"
#include "net_log.h"
#include "tcp_connection.h"
#include "event_loop.h"
#include "socket.h"
#include "../base/function.h"
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
    SystemLog_Debug("TCPClient ctor");

    connector_->set_callbakc_new_connection_(std::bind(&TCPClient::NewConnection, this, std::placeholders::_1));
    return;
}
//---------------------------------------------------------------------------
TCPClient::~TCPClient()
{
    SystemLog_Debug("TCPClient dtor");

    //析构的时候,connection必须是只有一个引用或者没有
    if(connection_)
    {
        {
        std::lock_guard<std::mutex> lock(mutex_);
        assert(connection_.unique());
        }

        assert(loop_ == connection_->owner_loop());
        connection_->ForceClose();
    }

    connector_->Stop();
    return;
}
//---------------------------------------------------------------------------
void TCPClient::Connect()
{
    SystemLog_Info("TCPClient:[%s] connect to %s", name_.c_str(), connector_->svr_addr().IPPort().c_str());

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
        connection_->Shutdown();
    }

    return;
}
//---------------------------------------------------------------------------
void TCPClient::stop()
{
    connect_ = false;
    connector_->Stop();

    return;
}
//---------------------------------------------------------------------------
void TCPClient::NewConnection(short sockfd)
{
    loop_->AssertInLoopThread();

    //服务器分配的ip port
    InetAddress peer_addr = Socket::GetPeerAddress(sockfd);
    std::string conn_name = base::CombineString("%s:%s#%d", name_.c_str(), peer_addr.IPPort().c_str(), next_conn_id_++);
    InetAddress local_addr(sockfd);
    TCPConnectionPtr conn(new TCPConnection(loop_, conn_name, sockfd, local_addr, peer_addr));
    conn->set_callback_connection(callback_connection_);
    conn->set_callback_read(callback_read_);
    conn->set_callback_write_complete(callback_write_complete_);
    conn->set_callback_disconnection(std::bind(&TCPClient::RemoveConnection, this, std::placeholders::_1));
    conn->Initialize();

    {
    std::lock_guard<std::mutex> lock(mutex_);
    connection_ = conn;
    }
    conn->ConnectionEstablished();
    return;
}
//---------------------------------------------------------------------------
void TCPClient::RemoveConnection(const TCPConnectionPtr& conn)
{
    loop_->AssertInLoopThread();
    assert(loop_ == conn->owner_loop());

    {
    std::lock_guard<std::mutex> lock(mutex_);
    assert(connection_.get() == conn.get());
    connection_.reset();
    }

    loop_->QueueInLoop(std::bind(&TCPConnection::ConnectionDestroy, conn));

    if(retry_ && connect_)
    {
        SystemLog_Info("connect[%s] reconnect to %s", name_.c_str(), connector_->svr_addr().IPPort().c_str());
        connector_->Restart();
    }

    return;
}
//---------------------------------------------------------------------------
}//namespace net
