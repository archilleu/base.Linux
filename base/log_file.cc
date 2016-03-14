//---------------------------------------------------------------------------
#include "log_file.h"
#include "function.h"
//---------------------------------------------------------------------------
namespace base
{

LogFile::LogFile()
:   log_level_(LOGLEVEL_DEBUG),
    output_console_(true),
    max_size_(UNIT_GB),
    running_(false),
    thread_(std::bind(&LogFile::OnThreadWriteLog, this), "Log file thread")
{
    append_list_.reserve(UNIT_KB);
    write_list_.reserve(UNIT_KB);
}
//---------------------------------------------------------------------------
LogFile::~LogFile()
{
    Uninitialze();
}
//---------------------------------------------------------------------------
bool LogFile::Initialze(const std::string& title, const std::string& path, const std::string& name, size_t max_size)
{
    if(title.empty())   return false;
    if(path.empty())    return false;
    if(name.empty())    return false;
    if(running_)        return true;

    if(false == FolderCreate(path, true))
        return false;

    title_      = title;
    path_       = path;
    name_       = name;
    max_size_   = max_size;

    running_ = true;
    if(false == thread_.Start())
    {
        running_ = false;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void LogFile::Uninitialze()
{
    if(false == running_)
        return;

    //通知线程退出
    running_ = false;
    {
    std::lock_guard<std::mutex> lock(mutex_);
    cond_.notify_one();
    }
    thread_.Join();

    //关闭文件
    if(log_file_)
    {
        log_file_->Close();
        log_file_.reset();
    }

    return;
}
//---------------------------------------------------------------------------
bool LogFile::WriteLog(int log_level, const std::string& log)
{
    return WriteLog(log_level, log.c_str());
}
//---------------------------------------------------------------------------
bool LogFile::WriteLog(int log_level, const char* log)
{
    if(false == running_)       return false;
    if(log_level_ > log_level)  return true;//设置输出等级比要输出的小,则不写log

    std::string format_log = MakeLogString(log_level, log);
    if(true == output_console_)
        fprintf(stderr, "%s\n", format_log.c_str());
    {
    std::unique_lock<std::mutex> lock(mutex_);
    append_list_.push_back(std::move(format_log));
    }
    cond_.notify_one();

    return true;
}
//---------------------------------------------------------------------------
void LogFile::CreateLogFile()
{
    if(log_file_)
        return;

    struct stat file_info;
    std::string file_name;
    std::string file_path;
    std::string date = Timestamp::Now().Date();
    for(int i=0; ; i++)
    {
        //拼接文件名
        std::ostringstream oss;
        oss << name_ << "_" << date << "(" << i << ")" << ".log";
        file_name = oss.str();

        //检查文件大小是否合适
        file_path = path_ + "/" + file_name;
        if(0 == stat(file_path.c_str(), &file_info))   //文件存在,文件不存在就不需要检查文件大小
        {
            if(max_size_ <= static_cast<size_t>(file_info.st_size))
                continue;
        }

        log_file_ = std::make_shared<AppendFile>();
        if(false == log_file_->Open(file_path))
        {
            log_file_.reset();
            break;
        }

        break;
    }

    //新文件,需要写标题
    if(0 == log_file_->Size())
        WriteLogFileTitle();
    
    return;
}
//---------------------------------------------------------------------------
void LogFile::WriteLogFileTitle()
{
    std::string title = title_ + "\r\n";
    if(log_file_)
        log_file_->Append(title.c_str(), title.size());

    return;
}
//---------------------------------------------------------------------------
std::string LogFile::MakeLogString(int log_level, const char* log)
{
    std::string level;
    switch(log_level)
    {
        case LOGLEVEL_DEBUG:
            level = "DEBUG";
            break;
        case LOGLEVEL_INFO:
            level = "NORMAL";
            break;
        case LOGLEVEL_WARNING:
            level = "WARNING";
            break;
        case LOGLEVEL_ERROR:
            level = "ERROR";
            break;
        default:
            level = "UNKNOWN";
            break;
    }
    return (Timestamp::Now().Datetime(true) + "<<" + level + ":" + log + "\r\n");
}
//---------------------------------------------------------------------------
void LogFile::OnThreadWriteLog()
{
    while(running_)
    {
        {
        std::unique_lock<std::mutex> lock(mutex_);
        while(32>append_list_.size())  //如果不是超时并且append_list_条数不足32条,则等下一次在写日志
        {
            if(std::cv_status::timeout == cond_.wait_for(lock, std::chrono::seconds(5)))
            {
                //如果列表不为空,不管收不到线程退出指令,把log写入到文件中
                if(!append_list_.empty())
                    break;

                //如果列表为空,并且线程收到退出指令,则可以直接退出线程了
                if(false == running_)
                    return;
            }
        }

        append_list_.swap(write_list_);
        }

        for(auto iter=write_list_.begin(); write_list_.end()!=iter; ++iter)
        {
            CreateLogFile();
            if(log_file_)
            {
                log_file_->Append(iter->data(), iter->size());
                if(max_size_ <= log_file_->Size())
                {
                    log_file_->Close();
                    log_file_.reset();
                }
            }
        }

        if(log_file_)
            log_file_->Flush();

        write_list_.clear();
    }

    return;
}
//---------------------------------------------------------------------------
}//namespace base
