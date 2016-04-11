//---------------------------------------------------------------------------
#include "test_computer_info.h"
#include "../computer_info.h"
//---------------------------------------------------------------------------
using namespace base;
using namespace base::test;
//---------------------------------------------------------------------------
bool TestComputerInfo::DoTest()
{
    if(false == Test_Normal())  return false;

    return true;
}
//---------------------------------------------------------------------------
bool TestComputerInfo::Test_Normal()
{
    //computer description
    {
    ComputerInfo::ComputerName name = ComputerInfo::GetComputerName();
    std::cout << "systeem name:" << name.sysname << std::endl <<
        "net name:" << name.netname << std::endl <<
        "release:" << name.release << std::endl <<
        "version" << name.version << std::endl <<
        "machine:" << name.machine << std::endl;
    }

    //filesystem
    {
    std::vector<ComputerInfo::DiskspaceInfo> info = ComputerInfo::GetDiskspaceInfo();
    std::cout << "filesystem " << "mount point" << "used " << "total " << std::endl;
    for(auto iter : info)
    {
        std::cout << iter.filesystem << " " << iter.mount_point << "   " << iter.used/base::UNIT_KB << "     " << iter.total/base::UNIT_KB << std::endl;
    }

    }

    //memory
    {
    ComputerInfo::MemoryInfo mem = ComputerInfo::GetMemoryInfo();
    std::cout << "mem_total:" << mem.mem_total << " mem_free: " << mem.mem_free << " swap_total:" << mem.swap_total << " swap_free: " << mem.swap_free << std::endl;
    }

    //cpu info
    {
    ComputerInfo::CPUInfo cpu = ComputerInfo::GetCPUInfo();
    std::cout << "vender:" << cpu.vender << std::endl <<
        "model name:" << cpu.modle_name << std::endl <<
        "MHz: " << cpu.MHz << std::endl <<
        "sockets:" << cpu.sockets << std::endl <<
        "core_per_socket:" << cpu.core_per_socket << std::endl <<
        "thread_per_core:" << cpu.thread_per_core << std::endl;
    }

    return true;
}
//---------------------------------------------------------------------------
