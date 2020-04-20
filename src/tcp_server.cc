//---------------------------------------------------------------------------
#include <algorithm>
#include "base/include/function.h"
#include "tcp_server.h"
#include "event_loop.h"
#include "acceptor.h"
#include "inet_address.h"
#include "socket.h"
#include "net_logger.h"
#include "tcp_connection.h"
#include "event_loop_thread_pool.h"
//---------------------------------------------------------------------------
namespace net
{

using namespace std::placeholders;

//---------------------------------------------------------------------------
static const size_t kConnSize = 1024 * 1024;
//---------------------------------------------------------------------------
TCPServer::TCPServer(EventLoop* owner_loop, const std::vector<InetAddress>& addresses)
:   mark_(0),
    owner_loop_(owner_loop),
    next_connect_id_(0),
    tcp_conn_count_(0),
    loop_thread_pool_(new EventLoopThreadPool(owner_loop))
{
    std::string msg = "ctor tcp server, listen address:";
    for(auto& addr : addresses)
    {
        msg += " " + addr.IpPort();
        acceptors_.push_back(std::make_shared<Acceptor>(owner_loop, addr));
        acceptors_.back()->set_new_conn_cb(std::bind(&TCPServer::OnNewConnection, this, 
                _1, _2, _3));
    }

    tcp_conn_list_.resize(kConnSize);
    NetLogger_trace("%s", msg.c_str());

    return;
}
//---------------------------------------------------------------------------
TCPServer::TCPServer(EventLoop* owner_loop, short port)
:   mark_(0),
    owner_loop_(owner_loop),
    next_connect_id_(0),
    tcp_conn_count_(0),
    loop_thread_pool_(new EventLoopThreadPool(owner_loop))
{
    NetLogger_trace("ctor tcp server, listen address:localhost");

    //ipv4、ipv6同时监听
    acceptors_.push_back(std::make_shared<Acceptor>(owner_loop, InetAddress(port, true)));
    acceptors_.push_back(std::make_shared<Acceptor>(owner_loop, InetAddress(port, false)));
    tcp_conn_list_.resize(kConnSize);
    acceptors_[0]->set_new_conn_cb(std::bind(&TCPServer::OnNewConnection, this, 
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    acceptors_[1]->set_new_conn_cb(std::bind(&TCPServer::OnNewConnection, this, 
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    return;
}
//---------------------------------------------------------------------------
TCPServer::~TCPServer()
{
    NetLogger_trace("dtor tcp server");

    return;
}
//---------------------------------------------------------------------------
void TCPServer::set_event_loop_nums(int nums)
{
    loop_thread_pool_->set_thread_nums(nums);    
}
//---------------------------------------------------------------------------
void TCPServer::Start()
{
    owner_loop_->AssertInLoopThread();
    NetLogger_info("TCPServer start");

    loop_thread_pool_->Start();

    for(auto& acceptor : acceptors_)
    {
        //确保在owner_loop线程(主线程)监听（有可能调用线程和owner_loop线程不在同一个）
        owner_loop_->RunInLoop(std::bind(&Acceptor::Listen, acceptor));
    }

    return;
}
//---------------------------------------------------------------------------
void TCPServer::Stop()
{
    owner_loop_->AssertInLoopThread();
    NetLogger_info("TCPServer stop");

    loop_thread_pool_->Stop();

    //断开所有连接
    for(auto& conn : tcp_conn_list_)
    {
        if(!conn)
            continue;

        //客户端的连接需要在自己的线程中回调销毁
        conn->owner_loop()->RunInLoop(std::bind(&TCPConnection::ConnectionDestroy, conn));
    }

    return;
}
//---------------------------------------------------------------------------
void TCPServer::DumpConnections() const
{
    owner_loop_->AssertInLoopThread();

    size_t count = 0;
    for(size_t i=0; i<tcp_conn_list_.size(); i++)
    {
        if(!tcp_conn_list_[i])
            continue;

        assert(((void)"conn_ptr fd != idx", tcp_conn_list_[i]->socket().fd() == static_cast<int>(i)));
        count++;
    }

    assert(((void)"conns no eq count", count == tcp_conn_count_));
    NetLogger_trace("has tcp connections:%zu", tcp_conn_count_);

    return;
}
//---------------------------------------------------------------------------
void TCPServer::OnNewConnection(Socket&& client, InetAddress&& client_addr, uint64_t accept_time)
{
    owner_loop_->AssertInLoopThread();

    //获取一个event_loop
    EventLoop* loop = loop_thread_pool_->GetNextEventLoop();

    std::string new_conn_name = base::CombineString("%zu", next_connect_id_++);
    InetAddress local_addr = Socket::GetLocalAddress(client.fd());

    NetLogger_trace("accept time:%s, new connection server name:[%s], fd:%d, total[%zu]- from :%s to :%s",
            base::Timestamp(accept_time).Datetime(true).c_str(), new_conn_name.c_str(), client.fd(),
            tcp_conn_count_, local_addr.IpPort().c_str(), client_addr.IpPort().c_str());

    TCPConnectionPtr conn_ptr = std::make_shared<TCPConnection>(loop, std::move(new_conn_name),
            std::move(client), std::move(local_addr), std::move(client_addr));

    //初始化连接
    conn_ptr->set_connection_cb(connection_cb_);
    conn_ptr->set_disconnection_cb(disconnection_cb_);
    conn_ptr->set_read_cb(read_cb_);
    conn_ptr->set_write_complete_cb(write_complete_cb_);
    conn_ptr->set_high_water_mark_cb(high_water_mark_cb_, mark_);
    conn_ptr->set_remove_cb(std::bind(&TCPServer::OnConnectionRemove, this, std::placeholders::_1));
    
    //加入到连接list中
    if(false == AddConnListItem(conn_ptr))
        return;

    conn_ptr->Initialize();
    
    //通知连接已经就绪,监听事件，在conn_ptr中通知,只有在加入到tcp_conn_list_中后才
    //允许该连接的事件,防止在未加入list前,该连接又close掉导致list里找不到该连接
    loop->RunInLoop(std::bind(&TCPConnection::ConnectionEstablished, conn_ptr)); 

    return;
}
//---------------------------------------------------------------------------
void TCPServer::OnConnectionRemove(const TCPConnectionPtr& conn_ptr)
{
    //该回调是由连接的线程回调上来的,但是销毁连接需要在TCPServer的线程中执行
    owner_loop_->RunInLoop(std::bind(&TCPServer::OnConnectionRemoveInLoop, this, conn_ptr));
    return;
}
//---------------------------------------------------------------------------
void TCPServer::OnConnectionRemoveInLoop(const TCPConnectionPtr& conn_ptr)
{
    //在TCPserver的线程销毁连接
    owner_loop_->AssertInLoopThread();

    NetLogger_trace("Connection remove, server total[%zu]- name:%s,  fd:%d, from :%s to :%s",
            tcp_conn_count_, conn_ptr->name().c_str(), conn_ptr->socket().fd(),
            conn_ptr->local_addr().IpPort().c_str(), conn_ptr->peer_addr().IpPort().c_str());

    DelConnListItem(conn_ptr);

    //通知connection已经销毁
    conn_ptr->owner_loop()->RunInLoop(std::bind(&TCPConnection::ConnectionDestroy, conn_ptr));

#ifdef _DEBUG
    DumpConnections();
#endif

    return;
}
//---------------------------------------------------------------------------
bool TCPServer::AddConnListItem(const TCPConnectionPtr& conn_ptr)
{
    if(tcp_conn_list_.size() <= static_cast<size_t>(conn_ptr->socket().fd()))
        tcp_conn_list_.resize(tcp_conn_list_.size()*2);

    int fd = conn_ptr->socket().fd();
    if(tcp_conn_list_.size() < static_cast<size_t>(fd))
    {
        NetLogger_warn("connection already max");
        return false;
    }

    if(tcp_conn_list_[fd])
    {
        NetLogger_error("connection:%s already exist!!!", conn_ptr->name().c_str());
        assert(0);
    }

    tcp_conn_list_[fd] = conn_ptr;
    tcp_conn_count_++;
    return true;
}
//---------------------------------------------------------------------------
void TCPServer::DelConnListItem(const TCPConnectionPtr& conn_ptr)
{
    int fd = conn_ptr->socket().fd();

    if(!tcp_conn_list_[fd])
    {
        NetLogger_error("connection:%s not exist!!!", conn_ptr->name().c_str());
        assert(0);
    }

    tcp_conn_list_[fd].reset();
    tcp_conn_list_[fd] = nullptr;
    tcp_conn_count_--;

    return;
}
//---------------------------------------------------------------------------

}//namespace net
