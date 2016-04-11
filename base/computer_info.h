//---------------------------------------------------------------------------
#ifndef LINUX_BASE_COMPUTER_INFO_H_
#define LINUX_BASE_COMPUTER_INFO_H_
//---------------------------------------------------------------------------
#include "share_inc.h"
//---------------------------------------------------------------------------
namespace base
{

class ComputerInfo 
{
public:
    ComputerInfo();
    ~ComputerInfo();

    //Computer description
    struct ComputerName
    {
        std::string sysname;    /* Name of the implementation of the operating system.  */
        std::string netname;    /* Name of this node on the network.  */
        std::string release;    /* Current release level of this implementation.  */
        std::string version;    /* Current version level of this release.  */
        std::string machine;    /* Name of the hardware type the system is running on.  */
    };
    static ComputerName GetComputerName();

    //harddisk
    struct DiskspaceInfo
    {
        std::string filesystem;
        std::string mount_point;
        size_t total;
        size_t used;
    };
    static std::vector<DiskspaceInfo> GetDiskspaceInfo();

    //memory(kb)
    struct MemoryInfo
    {
        size_t mem_total;
        size_t mem_free;
        size_t swap_total;
        size_t swap_free;
    };
    static MemoryInfo GetMemoryInfo();

    //cup info
    struct CPUInfo
    {
        std::string vender;             //cup vender
        std::string modle_name;         //cpu name
        float       MHz;                //frequency
        int         sockets;            //the number of physical socket
        int         core_per_socket;    //the core number of per cpu
        int         thread_per_core;    //the thread number of per core
    };
    static CPUInfo GetCPUInfo();

private:
    static size_t GetMemoryValue(const char* line, const char* field);
};

}//namespace base
//---------------------------------------------------------------------------
#endif //LINUX_BASE_COMPUTER_INFO_H_
