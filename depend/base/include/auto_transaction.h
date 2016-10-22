//---------------------------------------------------------------------------
#ifndef BASE_AUTO_TRANSACTION_H_
#define BASE_AUTO_TRANSACTION_H_
//---------------------------------------------------------------------------
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
#endif //BASE_AUTO_TRANSACTION_H_
