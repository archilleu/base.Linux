//---------------------------------------------------------------------------
#include "tcp_client.h"
#include "event_loop.h"
#include "tcp_connection.h"
#include "../thirdpart/base/include/function.h"
//---------------------------------------------------------------------------
namespace net
{

TCPClient::TCPClient(EventLoop* event_loop, const InetAddress& server_addr, const std::string name)
:   event_loop_(event_loop),
    retry_(false),
    connect_(false),
    next_conn_id_(0),
    name_(name),
    high_water_mark_(10240),
    connector_(new TCPConnector(event_loop, server_addr))
{
    NetLogger_trace("TCPClient(%p) ctor", this);
    connector_->set_new_conn_cb(std::bind(&TCPClient::NewConnection, this, std::placeholders::_1));

    return;
}
//---------------------------------------------------------------------------
TCPClient::~TCPClient()
{
    NetLogger_trace("TCPClient(%p) dtor", this);

    //未防止外部长久持有TCPConnection错误发送数据，析构的时候connection_
    //必须在外部没有引用
    if(connection_)
    {
        {
            std::lock_guard<std::mutex> guard(mutex_);
            assert(connection_.unique());
        }
    }

    if(connect_)
        Disconnect();

    return;
}
//---------------------------------------------------------------------------
void TCPClient::Connect()
{
    connect_ = true;
    connector_->Start();

    return;
}
//---------------------------------------------------------------------------
void TCPClient::Disconnect()
{
    connect_ = false;
    {
        std::lock_guard<std::mutex> guard(mutex_);
        if(connection_)
            connection_->ForceClose();
    }

    connector_->Stop();
    return;
}
//---------------------------------------------------------------------------
void TCPClient::NewConnection(int fd)
{
    event_loop_->AssertInLoopThread();

    InetAddress peer_addr = Socket::GetPeerAddress(fd);
    InetAddress local_addr= Socket::GetLocalAddress(fd);
    std::string conn_name = base::CombineString("%s:%s#%d",
            name_.c_str(), peer_addr.IpPort().c_str(), next_conn_id_++);
    TCPConnectionPtr conn_ptr = std::make_shared<TCPConnection>(event_loop_, 
            std::move(conn_name), Socket(fd), std::move(local_addr), std::move(peer_addr));
    conn_ptr->set_connection_cb(conn_cb_);
    conn_ptr->set_read_cb(read_cb_);
    conn_ptr->set_write_complete_cb(write_complete_cb_);
    conn_ptr->set_high_water_mark_cb(write_highwater_mark_cb_, high_water_mark_);
    conn_ptr->set_remove_cb(std::bind(&TCPClient::RemoveConnection, this, std::placeholders::_1));
    conn_ptr->Initialize();

    {
        std::lock_guard<std::mutex> lock(mutex_);
        connection_ = conn_ptr;
    }

    conn_ptr->ConnectionEstablished();
    return;
}
//---------------------------------------------------------------------------
void TCPClient::RemoveConnection(const TCPConnectionPtr& conn_ptr)
{
    event_loop_->AssertInLoopThread();
    assert(event_loop_ == conn_ptr->owner_loop());

    {
        std::lock_guard<std::mutex> lock(mutex_);
        assert(connection_.get() == conn_ptr.get());
        connection_.reset();
    }

    event_loop_->QueueInLoop(std::bind(&TCPConnection::ConnectionDestroy, conn_ptr));

    if(retry_ && connect_)
    {
        NetLogger_info("connect[%s] reconnect to %s", name_.c_str(),
                connector_->server_addr().IpPort().c_str());

        connector_->Restart();
    }
}
//---------------------------------------------------------------------------

}//namespace net
//---------------------------------------------------------------------------
