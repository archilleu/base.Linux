//---------------------------------------------------------------------------
#include "computer_info.h"
#include <mntent.h>
#include <sys/vfs.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
//---------------------------------------------------------------------------
namespace base
{
//---------------------------------------------------------------------------
unsigned long long ComputerInfo::last_[4] = {0};
//---------------------------------------------------------------------------
ComputerInfo::ComputerInfo()
{
}
//---------------------------------------------------------------------------
ComputerInfo::~ComputerInfo()
{
}
//---------------------------------------------------------------------------
ComputerInfo::ComputerName ComputerInfo::GetComputerName()
{
    ComputerName    name;
    struct utsname  u;
    if(-1 == uname(&u))
        return name;

    name.sysname = u.sysname;
    name.netname = u.nodename;
    name.release = u.release;
    name.version = u.version;
    name.machine = u.machine;
    return name;
}
//---------------------------------------------------------------------------
std::vector<ComputerInfo::DiskspaceInfo> ComputerInfo::GetDiskspaceInfo()
{
    std::vector<DiskspaceInfo> result;

    FILE* fd = setmntent("/etc/mtab", "r");
    if(0 == fd)
        return result;

    struct statfs sts;
    DiskspaceInfo info;
    for(struct mntent* entry=getmntent(fd); 0!=entry; entry=getmntent(fd))
    {
        if(0 == strcmp("rootfs", entry->mnt_fsname))
            continue;

        info.filesystem = entry->mnt_fsname;
        info.mount_point= entry->mnt_dir;

        if(0 != statfs(entry->mnt_dir, &sts))
        {
            info.total  = 0;
            info.used   = 0;
        }

        if(0 == sts.f_blocks)
            continue;

        info.used = (sts.f_blocks-sts.f_bfree) * sts.f_bsize;
        info.total= sts.f_blocks * sts.f_bsize;
        result.push_back(info);
    }

    endmntent(fd);
    return result;
}
//---------------------------------------------------------------------------
ComputerInfo::MemoryInfo ComputerInfo::GetMemoryInfo()
{
    //also can read from /proc/meminfo

    MemoryInfo mem_info;
    bzero(&mem_info, sizeof(MemoryInfo));

    struct sysinfo sys_info;
    if(0 > sysinfo(&sys_info))
        return mem_info;

    mem_info.mem_total  = sys_info.totalram * sys_info.mem_unit / UNIT_KB;
    mem_info.mem_free   = sys_info.freeram * sys_info.mem_unit / UNIT_KB;
    mem_info.swap_total = sys_info.totalswap * sys_info.mem_unit / UNIT_KB;
    mem_info.swap_free  = sys_info.freeswap * sys_info.mem_unit / UNIT_KB;
 
    return mem_info;
}
//---------------------------------------------------------------------------
ComputerInfo::CPUInfo ComputerInfo::GetCPUInfo()
{
    CPUInfo info= {"","",0,0,0,0};
    FILE*   fp  = fopen("/proc/cpuinfo", "r");
    if(0 == fp)
        return info;

    std::set<int>   sockets;  //socket count
    char            buffer[64];
    while(true)
    {
        //line format "field    : value",more info man /proc
        if(0 == fgets(buffer, sizeof(buffer), fp))
            break;

        if(info.vender.empty())
        {
            const char* vender = "vendor_id";
            if(0 == memcmp(buffer, vender, strlen(vender)))
            {
                char value[64];
                sscanf(buffer, "%*[^:]%*s%[^\n]", value);//because buffer's size limit 64b,so sscanf is save.
                info.vender = value;
                continue;
            }
        }

        if(info.modle_name.empty())
        {
            const char* model_name = "model name";
            if(0 == memcmp(buffer, model_name, strlen(model_name)))
            {
                char value[64];
                sscanf(buffer, "%*[^:]%*s%[^\n]", value);//because buffer's size limit 64b,so sscanf is save.
                info.modle_name = value;
                continue;
            }
        }

        if(0.1 >= info.MHz)
        {
            const char* MHz = "cpu MHz";
            if(0 == memcmp(buffer, MHz, strlen(MHz)))
            {
                float value;
                sscanf(buffer, "%*[^:]%*s%f", &value);
                info.MHz = value;
                continue;
            }
        }

        const char* physical_id = "physical id";
        if(0 == memcmp(buffer, physical_id, strlen(physical_id)))
        {
            int value;
            sscanf(buffer, "%*[^:]%*s%d", &value);
            sockets.insert(value);
            continue;
        }

        if(0 == info.core_per_socket)
        {
            const char* cpu_cores = "cpu cores";
            if(0 == memcmp(buffer, cpu_cores, strlen(cpu_cores)))
            {
                int value;
                sscanf(buffer, "%*[^:]%*s%d", &value);
                info.core_per_socket = value;    
                continue;
            }
        }

        if(0 == info.thread_per_core)
        {
            const char* siblings = "siblings";
            if(0 == memcmp(buffer, siblings, strlen(siblings)))
            {
                int value;
                sscanf(buffer, "%*[^:]%*s%d", &value);
                info.thread_per_core = value;    
                continue;
            }
        }
    }
    
    fclose(fp);
    info.sockets        = static_cast<int>((sockets.size() > 0) ? sockets.size() : 1);
    info.thread_per_core= info.thread_per_core / info.core_per_socket;
    return info;
}
//---------------------------------------------------------------------------
void ComputerInfo::InitCPUUsage()
{
    GetCPUValue(last_);
}
//---------------------------------------------------------------------------
float ComputerInfo::GetCPUUsage()
{
    unsigned long long current[4];
    GetCPUValue(current);

    float percent = 0.0;
    //prevent overflow
    if( (current[0]>=last_[0]) &&
        (current[1]>=last_[1]) &&
        (current[2]>=last_[2]) &&
        (current[3]>=last_[3]))
    {
        unsigned long long usage = (current[0]-last_[0]) + (current[1]-last_[1]) + (current[2]-last_[2]);
        unsigned long long total = usage + (current[3]-last_[3]);
        if(0 != total)
            percent = (static_cast<float>(usage)/static_cast<float>(total)) * 100;
    }

    //update last
    last_[0] = current[0];
    last_[1] = current[1];
    last_[2] = current[2];
    last_[3] = current[3];
     
    return percent;
}
//---------------------------------------------------------------------------
void ComputerInfo::GetCPUValue(unsigned long long value[4])
{
    bzero(value, sizeof(unsigned long long)*4);

    FILE* fp = fopen("/proc/stat", "r");
    if(0 == fp)
        return;
    
    int result = fscanf(fp, "cpu %llu %llu %llu %llu", &value[0], &value[1], &value[2], &value[3]);
    (void)result;
    fclose(fp);
}
//---------------------------------------------------------------------------
}//namespace base
