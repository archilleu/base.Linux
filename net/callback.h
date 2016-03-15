//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_CALLBACK_H_
#define BASE_LINUX_NET_CALLBACK_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
#include "../base/timestamp.h"
//---------------------------------------------------------------------------
namespace net
{

class TCPConnection;
class Buffer;

typedef std::shared_ptr<TCPConnection> TCPConnectionPtr;

typedef std::function<void (const TCPConnectionPtr&)>                           CallbackConnection;
typedef std::function<void (const TCPConnectionPtr&)>                           CallbackDisconnection;
typedef std::function<void (const TCPConnectionPtr&, Buffer&, base::Timestamp)> CallbackRead;
typedef std::function<void (const TCPConnectionPtr&)>                           CallbackWriteComplete;
typedef std::function<void (const TCPConnectionPtr&, size_t)>                   CallbackWriteHighWaterMark;

typedef std::function<void (void)>  CallbackTimerTask;

}//namespace net
//---------------------------------------------------------------------------
#endif //BASE_LINUX_NET_CALLBACK_H_
