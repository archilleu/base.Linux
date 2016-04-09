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

    //harddisk
    struct DiskspaceInfo
    {
        std::string filesystem;
        std::string mount_point;
        size_t total;
        size_t used;
    };
    static std::vector<DiskspaceInfo> GetDiskspaceInfo();

    //memory
    struct MemoryInfo
    {
        size_t mem_total;
        size_t mem_free;
        size_t swap_total;
        size_t swap_free;
    };
};

}//namespace base
//---------------------------------------------------------------------------
#endif //LINUX_BASE_COMPUTER_INFO_H_
