//---------------------------------------------------------------------------
#ifndef BASE_LOG_FILE_H_
#define BASE_LOG_FILE_H_ 
//---------------------------------------------------------------------------
#include <memory>
#include <string>
#include <vector>
#include <condition_variable>
#include "thread.h"
//---------------------------------------------------------------------------
namespace base
{

class AppendFile;
class LogFile

{
public:
    LogFile();
    ~LogFile();
    LogFile(const LogFile&) =delete;
    LogFile& operator=(const LogFile&) =delete;

    void set_log_level      (int level)     { log_level_ = level; }
    void set_output_console (bool output)   { output_console_ = output; }

    bool Initialze  (const std::string& title, const std::string& path, const std::string& name, size_t max_size=1024*1024);
    void Uninitialze();

    enum
    {
        LOGLEVEL_DEBUG =1,
        LOGLEVEL_INFO,
        LOGLEVEL_WARNING,
        LOGLEVEL_ERROR
    };
    bool WriteLog(int log_level, const std::string& log);
    bool WriteLog(int log_level, const char* log);

private:
    void        CreateLogFile       ();                                 //根据设定创建log文件
    void        WriteLogFileTitle   ();                                 //写log文件标题
    std::string MakeLogString       (int log_level, const char* log);   //拼接log格式

private:
    void OnThreadWriteLog();

private:
    int         log_level_;
    bool        output_console_;
    size_t      max_size_;
    std::string title_;
    std::string path_;
    std::string name_;
    
    bool                        running_;
    Thread                      thread_;
    std::mutex                  mutex_;
    std::condition_variable     cond_;
    std::vector<std::string>    append_list_;
    std::vector<std::string>    write_list_;
    std::shared_ptr<AppendFile> log_file_;
};

}// namespace base
//---------------------------------------------------------------------------
#endif //BASE_LOG_FILE_H_
