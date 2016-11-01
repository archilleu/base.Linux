//---------------------------------------------------------------------------
#ifndef NET_NET_LOG_H_
#define NET_NET_LOG_H_
//---------------------------------------------------------------------------
#include "../depend/base/include/logger.h"
#include "../depend/base/include/thread.h"
//---------------------------------------------------------------------------
#define NetLogger_trace(format, ...) logger->trace("[%s:%s:%d:%s]:" format " ", base::CurrentThread::tid_str(),__FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define NetLogger_debug(format, ...) logger->debug("[%s:%s:%d]:" format " ", base::CurrentThread::tid_str(),__FILE__, __LINE__, ##__VA_ARGS__)
#define NetLogger_info(...) logger->info(__VA_ARGS__)
#define NetLogger_warn(format, ...) logger->warn("[%s:%s:%d]:" format " ", base::CurrentThread::tid_str(), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define NetLogger_error(format, ...) logger->error("[%s:%s:%d]:" format " ", base::CurrentThread::tid_str(), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define NetLogger_critical(format, ...) logger->critical("[%s:%d]:" format " ", base::CurrentThread::tid_str(), __LINE__, ##__VA_ARGS__)
#define NetLogger_off(format, ...) logger->off("[%s:%d]:" format " ", base::CurrentThread::tid_str(), __LINE__, ##__VA_ARGS__)
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
const char* OSError(int e);
extern std::shared_ptr<base::Logger> logger;
//---------------------------------------------------------------------------

}//namespace net
//---------------------------------------------------------------------------
#endif //NET_NET_LOG_H_


