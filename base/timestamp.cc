//---------------------------------------------------------------------------
#include "timestamp.h"
#include <time.h>
#include <sys/time.h>
//---------------------------------------------------------------------------
namespace base
{
//---------------------------------------------------------------------------
Timestamp Timestamp::kZero = Timestamp();
//---------------------------------------------------------------------------
Timestamp::Timestamp(const std::string& datetime)
{
    struct tm time;
    int nums = sscanf(datetime.c_str(), "%4d-%02d-%02d %02d:%02d:%02d", &time.tm_year, &time.tm_mon, &time.tm_mday, &time.tm_hour, &time.tm_min, &time.tm_sec);
    if(3 > nums)
    {
        micro_seconds_ = 0;
        return;
    }
    time.tm_year    -= 1900;
    time.tm_mon     -= 1;
    micro_seconds_  = static_cast<uint64_t>(timegm(&time) * kMicrosecondsPerSecond);

    return;
}
//---------------------------------------------------------------------------
std::string Timestamp::Data()
{
    struct tm time;
    bzero(&time, sizeof(struct tm));
    time_t seconds = static_cast<time_t>(Secodes());
    gmtime_r(&seconds, &time);

    std::string datetime("YYYY-MM-DD");
    snprintf(const_cast<char*>(datetime.data()), datetime.size()+1, "%4d-%02d-%02d", time.tm_year+1900, time.tm_mon+1, time.tm_mday);

    //return std::move(datetime); RVO
    return datetime;
}
//---------------------------------------------------------------------------
std::string Timestamp::Time()
{
    struct tm time;
    bzero(&time, sizeof(struct tm));
    time_t seconds = static_cast<time_t>(Secodes());
    gmtime_r(&seconds, &time);

    std::string datetime("00:00:00");
    snprintf(const_cast<char*>(datetime.data()), datetime.size()+1, "%02d:%02d:%02d", time.tm_hour, time.tm_min, time.tm_sec);

    //return std::move(datetime); RVO
    return datetime;
}
//---------------------------------------------------------------------------
std::string Timestamp::Datatime(bool decimal)
{
    struct tm time;
    bzero(&time, sizeof(struct tm));
    time_t seconds = static_cast<time_t>(Secodes());
    gmtime_r(&seconds, &time);

    if(decimal) //show micro seconds
    {
        std::string datetime("YYYY-MM-DD HH:MM:SS.000000");
        snprintf(const_cast<char*>(datetime.data()), datetime.size()+1, "%4d-%02d-%02d %02d:%02d:%02d.%06d", 
            time.tm_year+1900, time.tm_mon+1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec, static_cast<int>(micro_seconds_%kMicrosecondsPerSecond));

        return datetime;//RVO
    }
    else
    {
        std::string datetime("YYYY-MM-DD HH:MM:SS");
        snprintf(const_cast<char*>(datetime.data()), datetime.size()+1, "%4d-%02d-%02d %02d:%02d:%02d", 
            time.tm_year+1900, time.tm_mon+1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);

        return datetime;//RVO
    }
}
//---------------------------------------------------------------------------
void Timestamp::AddTime(uint64_t seconds)
{
    micro_seconds_ += seconds * kMicrosecondsPerSecond;
    return;
}
//---------------------------------------------------------------------------
void Timestamp::ReduceTime(uint64_t seconds)
{
    micro_seconds_ -= seconds * kMicrosecondsPerSecond;
    return;
}
//---------------------------------------------------------------------------
Timestamp Timestamp::Now()
{
    struct timeval tv;
    gettimeofday(&tv, 0);

   return Timestamp(static_cast<uint64_t>(tv.tv_sec)*kMicrosecondsPerSecond + tv.tv_usec);//RVO
}
//---------------------------------------------------------------------------
Timestamp& Timestamp::Zero()
{
    return Timestamp::kZero;
}
//---------------------------------------------------------------------------
}//namespace base
