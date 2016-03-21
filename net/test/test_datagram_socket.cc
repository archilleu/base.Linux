//---------------------------------------------------------------------------
#include "test_datagram_socket.h"
//#include "../datagram_socket.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
bool TestDatagramSocket::DoTest()
{
    if(false == Test_Normal())  return false;

    return true;
}
//---------------------------------------------------------------------------
bool TestDatagramSocket::Test_Normal()
{
    return true;
}
//---------------------------------------------------------------------------
