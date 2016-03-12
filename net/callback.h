//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_CALLBACK_H_
#define BASE_LINUX_NET_CALLBACK_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
#include "../base/timestamp.h"
//---------------------------------------------------------------------------
namespace net
{
typedef std::function<void (void)>  CallbackTimerTask;
}//namespace net
//---------------------------------------------------------------------------
#endif //BASE_LINUX_NET_CALLBACK_H_
