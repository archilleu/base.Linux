//---------------------------------------------------------------------------
#include "net_log.h"
#include "../base/function.h"
//---------------------------------------------------------------------------
namespace net
{
//---------------------------------------------------------------------------
NetLog MyNetLog;
//---------------------------------------------------------------------------
NetLog::NetLog()
{
    if(false == MyNetLog.Initialize("net io frame", base::RunPathFolder(), "network_log", base::UNIT_MB))
    {
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
NetLog::~NetLog()
{
    log_file_.Uninitialze();
}
//---------------------------------------------------------------------------
bool NetLog::Initialize(const std::string title, const std::string path, const std::string name, size_t max_size)
{
    return log_file_.Initialze(title, path, name, max_size);
}
//---------------------------------------------------------------------------
void NetLog::Uninitialize()
{
    log_file_.Uninitialze();
}
//---------------------------------------------------------------------------
void NetLog::LogWrite(int log_type, const char* format, ...)
{
    va_list va;
    va_start(va, format);
        char* buffer = 0;
        int err_code = vasprintf(&buffer, format, va);
    va_end(va);

    if(0 > err_code)
        return;

    log_file_.WriteLog(log_type, buffer);
    free(buffer);

    return;
}
//---------------------------------------------------------------------------
}//namespace net
