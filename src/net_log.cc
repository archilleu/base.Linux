//---------------------------------------------------------------------------
#include <cstring>
#include <cstdarg>
#include "net_log.h"
#include "../depend/base/include/function.h"
//---------------------------------------------------------------------------
__thread char t_error_buf[512];
//---------------------------------------------------------------------------
const char* StrError(int e)
{
    return strerror_r(e, t_error_buf, sizeof(t_error_buf));
}
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
NetLog* MyNetLog = 0;
//---------------------------------------------------------------------------
NetLog::NetLog()
{
}
//---------------------------------------------------------------------------
NetLog::~NetLog()
{
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
