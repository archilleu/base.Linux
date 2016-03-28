//---------------------------------------------------------------------------
#include "test_mysql_kit.h"
#include "../mysql_kit.h"
#include "../function.h"
//---------------------------------------------------------------------------
using namespace base;
using namespace base::test;
//---------------------------------------------------------------------------
bool TestMysqlKit::DoTest()
{
    if(false == TestNormal())   return false;

    return true;
}
//---------------------------------------------------------------------------
bool TestMysqlKit::TestNormal()
{
    //连接数据库
    {
    base::MysqlKit mysql;
    MY_ASSERT(MysqlKit::SUCCESS == mysql.Connect("localhost", 0, "root", "", ""));
    mysql.Disconnect();
    }

    //创建DB
    {
    MysqlKit mysql;
    MY_ASSERT(MysqlKit::SUCCESS == mysql.Connect("localhost", 0, "root", "", ""));
 
    std::string sql = "CREATE DATABASE IF NOT EXISTS db_test";
    MY_ASSERT(MysqlKit::SUCCESS == mysql.Execute(sql));
    sql = "DROP DATABASE db_test";
    MY_ASSERT(MysqlKit::SUCCESS == mysql.Execute(sql));

    mysql.Disconnect();
    }

    //创建表
    {
    MysqlKit mysql;
    MY_ASSERT(MysqlKit::SUCCESS == mysql.Connect("localhost", 0, "root", "", ""));
 
    std::string sql = "CREATE DATABASE IF NOT EXISTS db_test";
    MY_ASSERT(MysqlKit::SUCCESS == mysql.Execute(sql));

    sql = "CREATE TABLE IF NOT EXISTS db_test.tb_test(AAA VARCHAR(20))";
    MY_ASSERT(MysqlKit::SUCCESS == mysql.Execute(sql));
    //插入数据
    std::string temp = "INSERT INTO db_test.tb_test (AAA) VALUES (\"%s\")";

    int err_no  = 0;
    int affe    = 0;
    MY_ASSERT(MysqlKit::SUCCESS == mysql.Execute(CombineString(temp.c_str(), "haha1"), &err_no, &affe));
    MY_ASSERT(0 == err_no);
    MY_ASSERT(1 == affe);

    MY_ASSERT(MysqlKit::SUCCESS == mysql.Execute(CombineString(temp.c_str(), "haha2"), &err_no, &affe));
    MY_ASSERT(0 == err_no);
    MY_ASSERT(1 == affe);

    MY_ASSERT(MysqlKit::SUCCESS == mysql.Execute(CombineString(temp.c_str(), "haha3"), &err_no, &affe));
    MY_ASSERT(0 == err_no);
    MY_ASSERT(1 == affe);
    MY_ASSERT(MysqlKit::SUCCESS == mysql.Execute(sql));

    int count   = 0;
    sql         = "SELECT COUNT(*) FROM db_test.tb_test";
    MY_ASSERT(MysqlKit::SUCCESS == mysql.GetRecordCount(sql, &count));
    MY_ASSERT(3 == count);

    MysqlKit::Records record;
    sql = "SELECT * FROM db_test.tb_test";
    MY_ASSERT(MysqlKit::SUCCESS == mysql.GetRecordOne(sql, &record));
    MY_ASSERT(1 == record.size());
    MY_ASSERT("haha1" == record[0]);

    MysqlKit::RecordList record_list;
    MY_ASSERT(MysqlKit::SUCCESS == mysql.GetRecord(sql, &record_list));
    MY_ASSERT(3 == record_list.size());
    for(auto iter1=record_list.begin(); iter1!=record_list.end(); ++iter1)
    {
        for(auto iter2=iter1->begin(); iter2!=iter1->end(); ++iter2)
            std::cout << *iter2 << std::endl;
    }
    
    record_list.clear();
    MY_ASSERT(MysqlKit::SUCCESS == mysql.GetRecord(sql, &record_list, 0, 1));
    MY_ASSERT(1 == record_list.size());
    for(auto iter1=record_list.begin(); iter1!=record_list.end(); ++iter1)
    {
        for(auto iter2=iter1->begin(); iter2!=iter1->end(); ++iter2)
            std::cout << *iter2 << std::endl;
    }

    record_list.clear();
    MY_ASSERT(MysqlKit::SUCCESS == mysql.GetRecord(sql, &record_list, 1, 1));
    MY_ASSERT(1 == record_list.size());
    for(auto iter1=record_list.begin(); iter1!=record_list.end(); ++iter1)
    {
        for(auto iter2=iter1->begin(); iter2!=iter1->end(); ++iter2)
            std::cout << *iter2 << std::endl;
    }

    record_list.clear();
    MY_ASSERT(MysqlKit::SUCCESS == mysql.GetRecord(sql, &record_list, 1, 3));
    MY_ASSERT(2 == record_list.size());
    for(auto iter1=record_list.begin(); iter1!=record_list.end(); ++iter1)
    {
        for(auto iter2=iter1->begin(); iter2!=iter1->end(); ++iter2)
            std::cout << *iter2 << std::endl;
    }

    //事务
    temp = "DELETE FROM db_test.tb_test WHERE AAA = \"%s\"";
    mysql.TransactionBegin();
        MY_ASSERT(mysql.Execute(CombineString(temp.c_str(), "haha1")));
    mysql.TransactionEnd();

    mysql.TransactionBegin();
        MY_ASSERT(mysql.Execute(CombineString(temp.c_str(), "haha2")));
        mysql.Execute("aa");
        record_list.clear();
        MY_ASSERT(MysqlKit::SUCCESS == mysql.GetRecord(sql, &record_list));
        MY_ASSERT(1 == record_list.size());
        for(auto iter1=record_list.begin(); iter1!=record_list.end(); ++iter1)
        {
            for(auto iter2=iter1->begin(); iter2!=iter1->end(); ++iter2)
                std::cout << *iter2 << std::endl;
        }
    mysql.TransactionEnd();

    record_list.clear();
    MY_ASSERT(MysqlKit::SUCCESS == mysql.GetRecord(sql, &record_list));
    MY_ASSERT(2 == record_list.size());
    for(auto iter1=record_list.begin(); iter1!=record_list.end(); ++iter1)
    {
        for(auto iter2=iter1->begin(); iter2!=iter1->end(); ++iter2)
            std::cout << *iter2 << std::endl;
    }

    sql = "DROP DATABASE db_test";
    mysql.Disconnect();
    }

    return true;
}
//---------------------------------------------------------------------------
