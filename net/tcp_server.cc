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
    acceptor_(new Acceptor(owner_loop, listen_addr)),
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
    loop_thread_pool_->Start();
    owner_loop_->RunInLoop(std::bind(&Acceptor::Listen, acceptor_));

    SystemLog_Debug("TCPServer start");
    return;
}
//---------------------------------------------------------------------------
void TCPServer::Stop()
{
    owner_loop_->AssertInLoopThread();

    loop_thread_pool_->Stop();

    //断开所有连接
    for(auto iter : tcp_name_connection_map_)
    {
        TCPConnectionPtr conn_ptr = iter.second;
        iter.second.reset();

        //客户端的连接需要在自己的线程中回调
        conn_ptr->owner_loop()->RunInLoop(std::bind(&TCPConnection::ConnectionDestroy, conn_ptr));
        conn_ptr.reset();
    }

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
            new_conn_name.c_str(), clientfd, tcp_name_connection_map_.size()+1, local_addr.IPPort().c_str(), client_addr.IPPort().c_str());

    TCPConnectionPtr conn_ptr = std::make_shared<TCPConnection>(loop, new_conn_name, clientfd, local_addr, client_addr);
    //初始化连接
    conn_ptr->set_callback_connection(callback_connection_);
    conn_ptr->set_callback_disconnection(callback_disconnection_);
    conn_ptr->set_callback_read(callback_read_);
    //销毁连接是由TCPConnection发起的回调,所以需要由TCPServer的线程自己来释放连接
    conn_ptr->set_callback_destroy(std::bind(&TCPServer::OnConnectionDestroy, this, std::placeholders::_1));
    
    //加入到连接map中
    tcp_name_connection_map_[new_conn_name] = conn_ptr;
    
    //只有在加入到map中后才允许该连接的事件,防止在未加入map前,该连接又close掉导致map里找不到该连接
    conn_ptr->Initialize();

    //通知连接已经就绪,在conn_ptr中通知
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
            name_.c_str(), tcp_name_connection_map_.size(), connection_ptr->name().c_str(), connection_ptr->socket()->fd(), connection_ptr->local_addr().IPPort().c_str(),
            connection_ptr->peer_addr().IPPort().c_str());

    size_t nums = tcp_name_connection_map_.erase(connection_ptr->name());
    if(0 == nums)
    {
        //因为channel disableall 设置成norml,导致接连收到close消息,坑自己~
        SystemLog_Warning("connection:%s not exist!!!", connection_ptr->name().c_str());
        assert(0);
    }

    //通知connection已经销毁
    connection_ptr->owner_loop()->RunInLoop(std::bind(&TCPConnection::ConnectionDestroy, connection_ptr));

    return;
}
//---------------------------------------------------------------------------
}//namespace net
