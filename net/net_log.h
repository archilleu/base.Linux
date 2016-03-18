//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_NET_LOG_H_
#define BASE_LINUX_NET_NET_LOG_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
#include "../base/log_file.h"
//---------------------------------------------------------------------------
#define SystemLog_Debug(format, ...)    (MyNetLog.LogWrite(base::LogFile::LOGLEVEL_DEBUG, "tid:%d,File:%s,Line[%d],Fun:%s >> " format " ", base::CurrentThread::Tid(),__FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__))
#define SystemLog_Info(format, ...)     (MyNetLog.LogWrite(base::LogFile::LOGLEVEL_INFO,"tid:%d,File:%s,Line[%d],Fun:%s >> " format " ", base::CurrentThread::Tid(),__FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__))
#define SystemLog_Warning(format, ...)  (MyNetLog.LogWrite(base::LogFile::LOGLEVEL_WARNING,"tid:%d,File:%s,Line[%d],Fun:%s >> " format " ", base::CurrentThread::Tid(),__FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__))
#define SystemLog_Error(format, ...)    (MyNetLog.LogWrite(base::LogFile::LOGLEVEL_ERROR, "tid:%d,File:%s,Line[%d],Fun:%s >> " format " ", base::CurrentThread::Tid(),__FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__))

namespace net
{

class NetLog
{
public:
    NetLog();
    ~NetLog();

    bool Initialize     (const std::string title, const std::string path, const std::string name, size_t max_size);
    void Uninitialize   ();

    void LogWrite(int log_type, const char* format, ...);

private:
    base::LogFile log_file_;    
};
//---------------------------------------------------------------------------
extern NetLog MyNetLog;
//---------------------------------------------------------------------------
}//namespace net
//---------------------------------------------------------------------------
#endif //BASE_LINUX_NET_NET_LOG_H_

