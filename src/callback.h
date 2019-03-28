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
class TCPConnection;
class Buffer;

using MemoryBlock = std::vector<char>;

using TCPConnectionPtr = std::shared_ptr<TCPConnection>;
using TCPConnectionWeakPtr = std::weak_ptr<TCPConnection>;

using ConnectionCallback = std::function<void (const TCPConnectionPtr&)>;
using DisconnectionCallback = std::function<void (const TCPConnectionPtr&)>;
using RemoveCallback = std::function<void (const TCPConnectionPtr&)>;
using ReadCallback = std::function<void (const TCPConnectionPtr&, Buffer&, uint64_t)>;
using WriteCompleteCallback = std::function<void (const TCPConnectionPtr&)>;
using WriteHighWaterMarkCallback = std::function<void (const TCPConnectionPtr&, size_t)>;

//timer task
using TimerCallback = std::function<void (void)>;

//UDP
class DatagramPacket;
using CallbackRcvPacket = std::function<void (const DatagramPacket& pkt)>;
}
//---------------------------------------------------------------------------
#endif //NET_CALLBACK_H_
