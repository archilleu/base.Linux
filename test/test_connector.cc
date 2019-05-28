//---------------------------------------------------------------------------
#include "test_inc.h"
#include "../src/tcp_connector.h"
#include "../src/event_loop.h"
#include "../src/inet_address.h"
#include "../src/socket.h"
#include <unistd.h>
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
namespace
{
    const char* SVR_IP  = "127.0.0.1";
    const short SVR_PORT= 9981; 
}
//---------------------------------------------------------------------------
void NewConnection(int fd)
{
    ::close(fd);
}
//---------------------------------------------------------------------------
bool Test_Normal()
{
    EventLoop loop;
    InetAddress inet_svr(SVR_IP, SVR_PORT);
    std::shared_ptr<TCPConnector> connector(new TCPConnector(&loop, inet_svr));
    connector->set_new_conn_cb(std::bind(NewConnection, std::placeholders::_1));

    std::cout << "svr addr:" << connector->server_addr().IpPort() << std::endl;
    connector->Start();
    loop.Loop();
    connector->Stop();
    loop.Quit();

    return true;
}
//---------------------------------------------------------------------------
int main()
{
    TEST_ASSERT(Test_Normal());

    return 0;
}
//---------------------------------------------------------------------------
