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
:   mark_(0),
    owner_loop_(owner_loop),
    name_(listen_addr.IPPort()),
    next_connect_id_(0),
    acceptor_(new Acceptor(owner_loop, listen_addr)),
    tcp_conn_count_(0),
    loop_thread_pool_(new EventLoopThreadPool(owner_loop, name_))
{
    SystemLog_Debug("ctor tcp server, listen address:%s", name_.c_str());

    acceptor_->set_callback_new_connection(std::bind(&TCPServer::OnNewConnection, this, 
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    return;
}
//---------------------------------------------------------------------------
TCPServer::~TCPServer()
{
    SystemLog_Debug("ctor tcp server, listen address:%s", name_.c_str());

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

    SystemLog_Info("TCPServer start");

    loop_thread_pool_->Start();
    owner_loop_->RunInLoop(std::bind(&Acceptor::Listen, acceptor_));

    return;
}
//---------------------------------------------------------------------------
void TCPServer::Stop()
{
    owner_loop_->AssertInLoopThread();

    SystemLog_Info("TCPServer sopt");

    loop_thread_pool_->Stop();

    //断开所有连接
    for(auto iter : tcp_conn_list_)
    {
        if(!iter)
            continue;

        TCPConnectionPtr conn_ptr = iter;
        iter.reset();

        //客户端的连接需要在自己的线程中回调
        //conn_ptr->owner_loop()->RunInLoop(std::bind(&TCPConnection::ConnectionDestroy, conn_ptr));
        conn_ptr.reset();
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

        assert(((void)"conn fd no eq idx", tcp_conn_list_[i]->socket()->fd() == static_cast<int>(i)));
        count++;
    }

    assert(((void)"conns no eq count", count == tcp_conn_count_));
    SystemLog_Debug("has tcp connections:%zu", tcp_conn_count_);

    return;
}
//---------------------------------------------------------------------------
void TCPServer::OnNewConnection(int clientfd, const InetAddress& client_addr, base::Timestamp accept_time)
{
    owner_loop_->AssertInLoopThread();

    //获取一个event_loop
    EventLoop* loop = loop_thread_pool_->GetNextEventLoop();

    std::string new_conn_name = base::CombineString("%s#%zu", name_.c_str(), next_connect_id_++);
    InetAddress local_addr = Socket::GetLocalAddress(clientfd);

    SystemLog_Debug("accept time:%s, new connection server name:[%s], fd:%d, total[%zu]- from :%s to :%s\n", accept_time.Datetime(true).c_str(),
            new_conn_name.c_str(), clientfd, tcp_conn_count_, local_addr.IPPort().c_str(), client_addr.IPPort().c_str());

    TCPConnectionPtr conn_ptr = std::make_shared<TCPConnection>(loop, new_conn_name, clientfd, local_addr, client_addr);
    //初始化连接
    conn_ptr->set_callback_connection(callback_connection_);
    conn_ptr->set_callback_disconnection(callback_disconnection_);
    conn_ptr->set_callback_read(callback_read_);
    conn_ptr->set_callback_write_complete(callback_write_complete_);
    conn_ptr->set_callback_high_water_mark(callback_high_water_mark_, mark_);
    //销毁连接是由TCPConnection发起的回调,需要由TCPServer的线程自己来释放连接
    conn_ptr->set_callback_destroy(std::bind(&TCPServer::OnConnectionDestroy, this, std::placeholders::_1));
    
    //加入到连接map中
    tcp_conn_list_[clientfd] = conn_ptr;
    conn_ptr->Initialize();
    
    //通知连接已经就绪,在conn_ptr中通知
    //只有在加入到list中后才允许该连接的事件,防止在未加入list前,该连接又close掉导致list里找不到该连接
    loop->RunInLoop(std::bind(&TCPConnection::ConnectionEstablished, conn_ptr)); 
    return;
}
//---------------------------------------------------------------------------
void TCPServer::OnConnectionDestroy(const TCPConnectionPtr& connection_ptr)
{
    //该回调是由连接的线程回调上来的,但是销毁连接需要在TCPServer的线程中执行(connection_ptr是拷贝的一个副本,即是是引用参数!!)
    owner_loop_->RunInLoop(std::bind(&TCPServer::OnConnectionDestroyInLoop, this, connection_ptr));
    return;
}
//---------------------------------------------------------------------------
void TCPServer::OnConnectionDestroyInLoop(const TCPConnectionPtr& connection_ptr)
{
    //在TCPserver的线程销毁连接
    owner_loop_->AssertInLoopThread();

    SystemLog_Debug("server name:[%s], total[%zu]- name:%s,  fd:%d, from :%s to :%s\n", 
            name_.c_str(), tcp_conn_count_, connection_ptr->name().c_str(), connection_ptr->socket()->fd(), connection_ptr->local_addr().IPPort().c_str(),
            connection_ptr->peer_addr().IPPort().c_str());

    if(!tcp_conn_list_[connection_ptr->socket()->fd()])
    {
        //当消息为<IN HUP ERR>时,该事件会被多次触发,所以TCPConnectioni做了相应修改(大部分断线只有<IN>消息)~,所以在TCPConnection直接remove省事点
        SystemLog_Warning("connection:%s not exist!!!", connection_ptr->name().c_str());
        assert(0);
    }
    tcp_conn_list_[connection_ptr->socket()->fd()].reset();

    //通知connection已经销毁
    //connection_ptr->owner_loop()->RunInLoop(std::bind(&TCPConnection::ConnectionDestroy, connection_ptr));
    return;
}
//---------------------------------------------------------------------------
}//namespace net
