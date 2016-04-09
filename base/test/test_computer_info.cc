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
    //filesystem
    {
        std::vector<ComputerInfo::DiskspaceInfo> info = ComputerInfo::GetDiskspaceInfo();
        std::cout << "filesystem " << "mount point" << "used " << "total " << std::endl;
        for(auto iter : info)
        {
            std::cout << iter.filesystem << " " << iter.mount_point << "   " << iter.used/base::UNIT_KB << "     " << iter.total/base::UNIT_KB << std::endl;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
