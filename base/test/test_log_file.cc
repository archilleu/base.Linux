//---------------------------------------------------------------------------
#include "test_log_file.h"
#include "../log_file.h"
//---------------------------------------------------------------------------
using namespace base;
using namespace base::test;
//---------------------------------------------------------------------------
const char* kLogDebug   = "hello, debug";
const char* kLogInfo    = "hello, info";
const char* kLogWarning = "hello, warning";
const char* kLogError   = "hello, error";
//---------------------------------------------------------------------------
bool TestLogFile::DoTest()
{
    if(false == Test_Illegal()) return false;
    if(false == Test_Size())    return false;
    if(false == Test_Date())    return false;

    return true;
}
//---------------------------------------------------------------------------
bool TestLogFile::Test_Illegal()
{
    //初始化失败
    {
    LogFile log_file;
    MY_ASSERT(false == log_file.Initialze("", "", "", 0));
    MY_ASSERT(false == log_file.Initialze("my title", "", "", 0));
    MY_ASSERT(false == log_file.Initialze("my title", "/tmp", "", 0));

    MY_ASSERT(false == log_file.WriteLog(LogFile::LOGLEVEL_DEBUG, "my first log"));
    }

    {
    const char* path = "/tmp/test_log";
    const char* name = "mylog";
    LogFile log_file;
    MY_ASSERT(true == log_file.Initialze("my title", path, name, UNIT_MB));
    for(int i=0; i<10; i++)
    {
        MY_ASSERT(true == log_file.WriteLog(LogFile::LOGLEVEL_DEBUG,      kLogDebug));
        MY_ASSERT(true == log_file.WriteLog(LogFile::LOGLEVEL_INFO,       kLogInfo));
        MY_ASSERT(true == log_file.WriteLog(LogFile::LOGLEVEL_WARNING,    kLogDebug));
        MY_ASSERT(true == log_file.WriteLog(LogFile::LOGLEVEL_ERROR,      kLogDebug));
    }
    }

    return true;
}
//---------------------------------------------------------------------------
bool TestLogFile::Test_Size()
{
    return true;
}
//---------------------------------------------------------------------------
bool TestLogFile::Test_Date()
{
    return true;
}
//---------------------------------------------------------------------------
