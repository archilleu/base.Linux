//---------------------------------------------------------------------------
#include <fcntl.h>
#include <poll.h>
#include "acceptor.h"
#include "event_loop.h"
#include "inet_address.h"
#include "channel.h"
#include "socket.h"
#include "net_logger.h"
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
Acceptor::Acceptor(EventLoop* event_loop, const InetAddress& addr_listen)
:   event_loop_(event_loop),
    idle_fd_(::open("/dev/null", O_RDONLY|O_CLOEXEC))
{
    NetLogger_trace("Acceptor(%p) ctor", this);

    if(0 > idle_fd_)
    {
        NetLogger_off("open /dev/null failed, errno:%d, msg:%s", errno, OSError(errno));
        exit(errno);
    }

    if(addr_listen.IsV4())
    {
        listen_socket_ = std::make_shared<Socket>(::socket(AF_INET,
                    SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, 0));
    }
    else
    {
        listen_socket_ = std::make_shared<Socket>(::socket(AF_INET6,
                    SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, 0));
        listen_socket_->SetIPV6Only();
    }

    if(0 > listen_socket_->fd())
    {
        NetLogger_off("listen sock create failed, errno:%d, msg:%s", errno, OSError(errno));
        exit(errno);
    }
    listen_socket_->SetReuseAddress();
    listen_socket_->Bind(addr_listen);

    listen_channel_ = std::make_shared<Channel>(event_loop_, listen_socket_->fd(), "acceptor");
    listen_channel_->set_read_cb(std::bind(&Acceptor::HandleRead, this,std::placeholders::_1));

    return;
}
//---------------------------------------------------------------------------
Acceptor::~Acceptor()
{
    NetLogger_trace("Acceptor(%p)", this);

    listen_channel_->DisableAll();
    listen_channel_->Remove();
    ::close(idle_fd_);

    return;
}
//---------------------------------------------------------------------------
void Acceptor::Listen()
{
    NetLogger_info("Acceptor listen");
    event_loop_->AssertInLoopThread();

    if(0 > ::listen(listen_socket_->fd(), SOMAXCONN))
    {
        NetLogger_off("listen failed, errno:%d, msg:%s", errno, OSError(errno));
        abort();
    }

    listen_channel_->EnableReading();
    return;
}
//---------------------------------------------------------------------------
int Acceptor::AcceptConnection(InetAddress& addr_peer)
{
    sockaddr_storage client;
    socklen_t len = sizeof(client);
    int client_fd = ::accept4(listen_socket_->fd(), reinterpret_cast<sockaddr*>(&client),
            &len, SOCK_NONBLOCK|SOCK_CLOEXEC);
    if(0 > client_fd)
    {
        if(EAGAIN == errno)
            return client_fd;

        NetLogger_error("accept failed, errno:%d, msg:%s", errno, OSError(errno));
        return -1;
    }

    addr_peer = InetAddress(client);
    return client_fd;
}
//---------------------------------------------------------------------------
void Acceptor::HandleRead(uint64_t rcv_time)
{
    for(;;)
    {
        InetAddress addr_peer;
        int client_fd = AcceptConnection(addr_peer);
        if(0 < client_fd)
        {
            if(false == CheckConnection(client_fd))
            {
                NetLogger_error("bad connectioin fd:%d, close", client_fd);
                ::close(client_fd);
                continue;
            }

            //如果不处理连接事件则关闭
            if(!new_conn_cb_ && !new_conn_data_cb_)
            {
                ::close(client_fd);
                continue;
            }

            Socket socket(client_fd);
            socket.SetKeepAlive(30);
            if(new_conn_cb_)
            {
                new_conn_cb_(std::move(socket), std::move(addr_peer), rcv_time);
            }
            else
            {
                new_conn_data_cb_(std::move(socket), std::move(addr_peer), rcv_time, config_data_);
            }
        }
        else
        {
            if(EAGAIN == errno)
                break;

            if(EMFILE == errno)
            {
                ::close(idle_fd_);
                idle_fd_ = AcceptConnection(addr_peer);
                ::close(idle_fd_);
                idle_fd_ = ::open("/dev/null", O_RDONLY|O_CLOEXEC);
            }

            NetLogger_error("accept failed, errno:%d, msg:%s", errno, OSError(errno));
            return;
        }
    }

    return;
}
//---------------------------------------------------------------------------
bool Acceptor::CheckConnection(int fd)
{
    //正常情况下是可写的，如果不可写则连接有问题
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLOUT;
    pfd.revents = 0;

    int num = poll(&pfd, 1, 0);
    if(1 == num)
    {
        if(POLLOUT & pfd.revents)
            return true;
    }

    return false;
}
//---------------------------------------------------------------------------

}//namespace net
//---------------------------------------------------------------------------
