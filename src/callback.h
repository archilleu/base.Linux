//---------------------------------------------------------------------------
#ifndef NET_CALLBACK_H_
#define NET_CALLBACK_H_
//---------------------------------------------------------------------------
#include <vector>
#include <memory>
#include <functional>
//---------------------------------------------------------------------------
namespace net
{

//TCP
class TCPConn;
class Buffer;

using MemoryBlock = std::vector<char>;

using TCPConnPtr = std::shared_ptr<TCPConn>;

using CallbackConnection            = std::function<void (const TCPConnPtr&)>;
using CallbackDisconnection         = std::function<void (const TCPConnPtr&)>;
using CallbackRemove                = std::function<void (const TCPConnPtr&)>;
using CallbackRead                  = std::function<void (const TCPConnPtr&, Buffer&, uint64_t)>;
using CallbackWriteComplete         = std::function<void (const TCPConnPtr&)>;
using CallbackWriteHighWaterMark    = std::function<void (const TCPConnPtr&, size_t)>;

//Task
using CallbackTimerTask = std::function<void (void)>;

//UDP
class DatagramPacket;
using CallbackRcvPacket = std::function<void (const DatagramPacket& pkt)>;
}//namespace net
//---------------------------------------------------------------------------
#endif //NET_CALLBACK_H_
