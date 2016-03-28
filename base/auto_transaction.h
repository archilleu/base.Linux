//---------------------------------------------------------------------------
#ifndef LINUX_BASE_AUTO_TRANSACTION_H_
#define LINUX_BASE_AUTO_TRANSACTION_H_
//---------------------------------------------------------------------------
#include "share_inc.h"
#include "mysql_kit.h"
//---------------------------------------------------------------------------
namespace base
{

class AutoTransatcion
{
public:
    AutoTransatcion(MysqlKit* mysql)
    :   mysql_(mysql)
    {
        mysql_->TransactionBegin();
        return;
    }
    ~AutoTransatcion()
    {
        mysql_->TransactionEnd();
        return;
    }

private:
    MysqlKit* mysql_;
};

}//namespace base
//---------------------------------------------------------------------------
#endif //LINUX_BASE_AUTO_TRANSACTION_H_
