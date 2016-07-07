//---------------------------------------------------------------------------
#include "test_connector.h"
#include "../connector.h"
#include "../event_loop.h"
#include "../inet_address.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
bool TestConnector::DoTest()
{
    if(false == Test_Normal())  return false;

    return true;
}
//---------------------------------------------------------------------------
namespace
{
    //const char* SVR_IP  = "127.0.0.1";
   // const short SVR_PORT= 9981; 
}
//---------------------------------------------------------------------------
void TestConnector::NewConnection(int sockfd)
{
    ::close(sockfd);
}
//---------------------------------------------------------------------------
bool TestConnector::Test_Normal()
{
    //EventLoop loop;
    //InetAddress inet_svr(SVR_IP, SVR_PORT);
    //std::shared_ptr<Connector> connector(new Connector(&loop, inet_svr));
    //connector->set_callbakc_new_connection_(std::bind(&TestConnector::NewConnection, this, std::placeholders::_1));

    //std::cout << "svr addr:" << connector->svr_addr().IPPort() << std::endl;
    //connector->Start();
    //loop.Loop();
    //connector->Stop();
    //loop.Quit();

    return true;
}
//---------------------------------------------------------------------------
