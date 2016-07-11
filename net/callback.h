//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_CALLBACK_H_
#define BASE_LINUX_NET_CALLBACK_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
#include "../base/timestamp.h"
//---------------------------------------------------------------------------
namespace net
{
//TCP
class TCPConn;
class Buffer;

typedef std::shared_ptr<TCPConn> TCPConnPtr;

typedef std::function<void (const TCPConnPtr&)>                           CallbackConnection;
typedef std::function<void (const TCPConnPtr&)>                           CallbackDisconnection;
typedef std::function<void (const TCPConnPtr&)>                           CallbackRemove;
typedef std::function<void (const TCPConnPtr&, Buffer&, base::Timestamp)> CallbackRead;
typedef std::function<void (const TCPConnPtr&)>                           CallbackWriteComplete;
typedef std::function<void (const TCPConnPtr&, size_t)>                   CallbackWriteHighWaterMark;

typedef std::function<void (void)>  CallbackTimerTask;

//UDP
class DatagramPacket;

typedef std::function<void (const DatagramPacket& pkt)> CallbackRcvPacket;
}//namespace net
//---------------------------------------------------------------------------
#endif //BASE_LINUX_NET_CALLBACK_H_
