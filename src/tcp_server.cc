//---------------------------------------------------------------------------
#include "tcp_server.h"
#include "event_loop.h"
#include "acceptor.h"
#include "inet_address.h"
#include "socket.h"
#include "net_log.h"
#include "tcp_connection.h"
#include "event_loop_thread_pool.h"
#include "../depend/base/include/function.h"
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
TCPServer::TCPServer(EventLoop* owner_loop, const std::vector<InetAddress>& listen_addr)
:   mark_(0),
    owner_loop_(owner_loop),
    next_connect_id_(0),
    tcp_conn_count_(0),
    loop_thread_pool_(new EventLoopThreadPool(owner_loop))
{
    std::string msg = "ctor tcp server";
    for(auto iter : listen_addr)
    {
        msg += " " + iter.IPPort();
        acceptor_.push_back(std::make_shared<Acceptor>(owner_loop, iter));
        acceptor_.back()->set_callback_new_connection(std::bind(&TCPServer::OnNewConnection, this, 
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

    tcp_conn_list_.resize(1024*1024);
    NetLogger_info("%s", msg.c_str());

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
    NetLogger_info("ctor tcp server, listen address:*");

    acceptor_.push_back(std::make_shared<Acceptor>(owner_loop, InetAddress(port, true)));
    acceptor_.push_back(std::make_shared<Acceptor>(owner_loop, InetAddress(port, false)));
    tcp_conn_list_.resize(1024*1024);
    acceptor_[0]->set_callback_new_connection(std::bind(&TCPServer::OnNewConnection, this, 
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    acceptor_[1]->set_callback_new_connection(std::bind(&TCPServer::OnNewConnection, this, 
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    return;
}
//---------------------------------------------------------------------------
TCPServer::~TCPServer()
{
    NetLogger_info("ctor tcp server");

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

    for(auto iter : acceptor_)
    {
        owner_loop_->RunInLoop(std::bind(&Acceptor::Listen, iter));
    }

    return;
}
//---------------------------------------------------------------------------
void TCPServer::Stop()
{
    owner_loop_->AssertInLoopThread();

    NetLogger_info("TCPServer sopt");

    loop_thread_pool_->Stop();

    //断开所有连接
    for(auto iter : tcp_conn_list_)
    {
        if(!iter)
            continue;

        TCPConnPtr conn_ptr = iter;
        iter.reset();

        //客户端的连接需要在自己的线程中回调
        conn_ptr->owner_loop()->RunInLoop(std::bind(&TCPConn::ConnectionDestroy, conn_ptr));
    }

    return;
}
//---------------------------------------------------------------------------
void TCPServer::DumpConnection()
{
    owner_loop_->AssertInLoopThread();

    size_t count = 0;
    for(size_t i=0; i<tcp_conn_list_.size(); i++)
    {
        if(!tcp_conn_list_[i])
            continue;

        assert(((void)"conn_ptr fd no eq idx", tcp_conn_list_[i]->socket()->fd() == static_cast<int>(i)));
        count++;
    }

    assert(((void)"conns no eq count", count == tcp_conn_count_));
    NetLogger_trace("has tcp connections:%zu", tcp_conn_count_);

    return;
}
//---------------------------------------------------------------------------
void TCPServer::OnNewConnection(int clientfd, const InetAddress& client_addr, uint64_t accept_time)
{
    owner_loop_->AssertInLoopThread();

    //获取一个event_loop
    EventLoop* loop = loop_thread_pool_->GetNextEventLoop();

    std::string new_conn_name = base::CombineString("%zu", next_connect_id_++);
    InetAddress local_addr = Socket::GetLocalAddress(clientfd);

    NetLogger_trace("accept time:%s, new connection server name:[%s], fd:%d, total[%zu]- from :%s to :%s", base::Timestamp(accept_time).Datetime(true).c_str(),
            new_conn_name.c_str(), clientfd, tcp_conn_count_, local_addr.IPPort().c_str(), client_addr.IPPort().c_str());

    TCPConnPtr conn_ptr = std::make_shared<TCPConn>(loop, new_conn_name, clientfd, local_addr, client_addr);
    //初始化连接
    conn_ptr->set_callback_connection(callback_connection_);
    conn_ptr->set_callback_disconnection(callback_disconnection_);
    conn_ptr->set_callback_read(callback_read_);
    conn_ptr->set_callback_write_complete(callback_write_complete_);
    conn_ptr->set_callback_high_water_mark(callback_high_water_mark_, mark_);
    conn_ptr->set_callback_remove(std::bind(&TCPServer::OnConnectionRemove, this, std::placeholders::_1));
    
    //加入到连接list中
    ConnAddList(conn_ptr);

    conn_ptr->Initialize();
    
    //通知连接已经就绪,在conn_ptr中通知
    //只有在加入到list中后才允许该连接的事件,防止在未加入list前,该连接又close掉导致list里找不到该连接
    loop->RunInLoop(std::bind(&TCPConn::ConnectionEstablished, conn_ptr)); 

    return;
}
//---------------------------------------------------------------------------
void TCPServer::OnConnectionRemove(const TCPConnPtr& conn_ptr)
{
    //该回调是由连接的线程回调上来的,但是销毁连接需要在TCPServer的线程中执行(conn_ptr是拷贝的一个副本,即是是引用参数!!)
    owner_loop_->RunInLoop(std::bind(&TCPServer::OnConnectionRemoveInLoop, this, conn_ptr));
    return;
}
//---------------------------------------------------------------------------
void TCPServer::OnConnectionRemoveInLoop(const TCPConnPtr& conn_ptr)
{
    //在TCPserver的线程销毁连接
    owner_loop_->AssertInLoopThread();

    NetLogger_trace("server total[%zu]- name:%s,  fd:%d, from :%s to :%s",
            tcp_conn_count_, conn_ptr->name().c_str(), conn_ptr->socket()->fd(), conn_ptr->local_addr().IPPort().c_str(),
            conn_ptr->peer_addr().IPPort().c_str());

    ConnDelList(conn_ptr);

    //通知connection已经销毁
    conn_ptr->owner_loop()->RunInLoop(std::bind(&TCPConn::ConnectionDestroy, conn_ptr));

#ifdef _DEBUG
    DumpConnection();
#endif

    return;
}
//---------------------------------------------------------------------------
void TCPServer::ConnAddList(const TCPConnPtr& conn_ptr)
{
    if(tcp_conn_list_.size() <= static_cast<size_t>(conn_ptr->socket()->fd()))
        tcp_conn_list_.resize(tcp_conn_list_.size()*2);

    if(tcp_conn_list_[conn_ptr->socket()->fd()])
    {
        NetLogger_warn("connection:%s already exist!!!", conn_ptr->name().c_str());
        assert(0);
    }

    tcp_conn_list_[conn_ptr->socket()->fd()] = conn_ptr;
    tcp_conn_count_++;
    return;
}
//---------------------------------------------------------------------------
void TCPServer::ConnDelList(const TCPConnPtr& conn_ptr)
{
    if(!tcp_conn_list_[conn_ptr->socket()->fd()])
    {
        NetLogger_warn("connection:%s not exist!!!", conn_ptr->name().c_str());
        assert(0);
    }

    tcp_conn_list_[conn_ptr->socket()->fd()].reset();
    tcp_conn_count_--;
    return;
}
//---------------------------------------------------------------------------

}//namespace net
