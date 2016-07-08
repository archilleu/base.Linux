//---------------------------------------------------------------------------
#ifndef LINUX_BASE_SHARE_DFE_H_
#define LINUX_BASE_SHARE_DFE_H_
//---------------------------------------------------------------------------
#include <stdint.h>
//---------------------------------------------------------------------------
#define DISALLOW_COPY_AND_ASSIGN(TypeName)  \
    TypeName(const TypeName&);              \
    void operator=(const TypeName&);

//---------------------------------------------------------------------------
namespace base
{
    const uint64_t UNIT_KB = 1024;
    const uint64_t UNIT_MB = 1024 * UNIT_KB;
    const uint64_t UNIT_GB = 1024 * UNIT_MB;
    
}//namespace base
//---------------------------------------------------------------------------
#endif// LINUX_BASE_SHARE_DFE_H_
