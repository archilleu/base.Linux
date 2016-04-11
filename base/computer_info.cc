//---------------------------------------------------------------------------
#include "computer_info.h"
#include <mntent.h>
#include <sys/vfs.h>
#include <sys/utsname.h>
//---------------------------------------------------------------------------
namespace base
{
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
    MemoryInfo info;
    bzero(&info, sizeof(MemoryInfo));

    FILE* fp = fopen("/proc/meminfo", "r");
    if(0 == fp)
        return info;

    char buffer[64];
    while(true)
    {
        //line format "field:   value",more info man /proc
        if(0 == fgets(buffer, sizeof(buffer), fp))
            break;

        if(0 == info.mem_total)
        {
            info.mem_total = GetMemoryValue(buffer, "MemTotal");
            continue;
        }

        if(0 == info.mem_free)
        {
            info.mem_free = GetMemoryValue(buffer, "MemFree");
            continue;
        }

        if(0 == info.swap_total)
        {
            info.swap_total = GetMemoryValue(buffer, "SwapTotal");
            continue;
        }

        if(0 == info.swap_free)
        {
            info.swap_free = GetMemoryValue(buffer, "SwapFree");
            continue;
        }
    }

    fclose(fp);
    return info;
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

    info.sockets        = static_cast<int>((sockets.size() > 0) ? sockets.size() : 1);
    info.thread_per_core= info.thread_per_core / info.core_per_socket;
    return info;
}
//---------------------------------------------------------------------------
size_t ComputerInfo::GetMemoryValue(const char* line, const char* field)
{
    if(0 != memcmp(field, line, strlen(field)))
        return 0;
                
    unsigned long value = 0;
    if(1 != sscanf(line, "%*s%lu", &value))
        return 0;
    
    return value;
}
//---------------------------------------------------------------------------
}//namespace base
