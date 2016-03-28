//---------------------------------------------------------------------------
#include "test_mysql_pool.h"
#include "../mysql_pool.h"
//---------------------------------------------------------------------------
using namespace base;
using namespace base::test;
//---------------------------------------------------------------------------
bool TestMysqlPool::DoTest()
{
    if(false == Test_Normal())  return false;

    return true;
}
//---------------------------------------------------------------------------
bool TestMysqlPool::Test_Normal()
{   
    MysqlPool pool("localhost", 0, "root", "", "");

    std::shared_ptr<MysqlKit> mysql = pool.Get();
    MY_ASSERT(0 != mysql.get());
    pool.Put(mysql);
    std::shared_ptr<MysqlKit> mysql1 = pool.Get();
    MY_ASSERT(mysql == mysql1);

    return true;
}
//---------------------------------------------------------------------------
