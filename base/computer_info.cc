//---------------------------------------------------------------------------
#include "computer_info.h"
#include <mntent.h>
#include <sys/vfs.h>
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
}//namespace base
