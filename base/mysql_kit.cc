//---------------------------------------------------------------------------
#include "mysql_kit.h"
#include "function.h"
#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>
//---------------------------------------------------------------------------
namespace base 
{
//---------------------------------------------------------------------------
 const int MysqlKit::kRecordMax     = UNIT_KB*8;
 const int MysqlKit::kDupEntry      = ER_DUP_ENTRY;
 const int MysqlKit::kNoReferenced  = ER_NO_REFERENCED_ROW_2;
//---------------------------------------------------------------------------
MysqlKit::MysqlKit()
:   port_(0),
    in_transaction_(false),
    transaction_result_(false),
    mysql_(0)
{
}
//---------------------------------------------------------------------------
MysqlKit::~MysqlKit()
{
}
//---------------------------------------------------------------------------
bool MysqlKit::MysqlKit::Connect(const std::string& host, short port, const std::string& user, const std::string& password, const std::string& database, const std::string& charset)
{
    if(host.empty())    return false;
    if(user.empty())    return false;

    MYSQL* mysql = mysql_init(0);
    if(0 == mysql)
        return false;

    if(0 == mysql_real_connect(mysql, host.c_str(), user.c_str(), password.c_str(), database.c_str(), port, 0, CLIENT_MULTI_STATEMENTS))
        goto WORK_ERROR;

    if(0 != mysql_autocommit(mysql, 0))
        goto WORK_ERROR;

    if(0 != mysql_query(mysql, "SET SESSION TRANSACTION ISOLATION LEVEL READ COMMITTED"))
        goto WORK_ERROR;
    
    if(0 != mysql_query(mysql, ("SET NAMES " + charset).c_str()))
        goto WORK_ERROR;

    mysql_              = mysql;
    host_               = host;
    port_               = port;
    user_               = user;
    password_           = password;
    database_           = database;
    charset_            = charset;
    in_transaction_     = false;
    transaction_result_ = false;
    return true;

WORK_ERROR:
    if(0 != mysql)  mysql_close(mysql);
    return false;
}
//---------------------------------------------------------------------------
void MysqlKit::Disconnect()
{
    if(0 != mysql_)
    {
        mysql_close(reinterpret_cast<MYSQL*>(mysql_));
        mysql_ = 0;
    }

    host_               = "";
    port_               = 0;
    user_               = "";
    password_           = "";
    database_           = "";
    charset_            = "utf8";
    in_transaction_     = false;
    transaction_result_ = false;

    return;
}
//---------------------------------------------------------------------------
int MysqlKit::Execute(const std::string& sql, int* err_no, int* affected_rows, uint64_t* last_insert_id)
{
    return Execute(sql.c_str(), err_no, affected_rows, last_insert_id);
}
//---------------------------------------------------------------------------
int MysqlKit::Execute(const char* sql, int* err_no, int* affected_rows, uint64_t* last_insert_id)
{
    if(0 == sql)
    {
        transaction_result_ = false;
        return ERROR;
    }

    //如果在事务期间,先前的sql失败,则直接返回失败
    if(true == in_transaction_)
    {
        if(false == transaction_result_)
            return ERROR;
    }

    if(0 != mysql_query(reinterpret_cast<MYSQL*>(mysql_), sql))
    {
        transaction_result_ = false;
        OnError(std::string("mysql_query:") + mysql_error(reinterpret_cast<MYSQL*>(mysql_)));
        if(0 != err_no) *err_no = mysql_errno(reinterpret_cast<MYSQL*>(mysql_));
        return ERROR;
    }

    if(0 != affected_rows)
        *affected_rows = static_cast<int>(reinterpret_cast<MYSQL*>(mysql_)->affected_rows);
    if(0 != last_insert_id)
        *last_insert_id = static_cast<uint64_t>(reinterpret_cast<MYSQL*>(mysql_)->insert_id);

    //如果不是在事务中,则提交变更
    if(false == in_transaction_)
    {
        if(0 != mysql_commit(reinterpret_cast<MYSQL*>(mysql_)))
        {
            OnError(std::string("mysql_commit:") + mysql_error(reinterpret_cast<MYSQL*>(mysql_)));
            if(0 != err_no) *err_no = mysql_errno(reinterpret_cast<MYSQL*>(mysql_));
            return ERROR;
        }
    }

    return SUCCESS;
}
//---------------------------------------------------------------------------
int MysqlKit::GetRecordCount(const std::string& sql, int* result)
{
    return GetRecordCount(sql.c_str(), result);
}
//---------------------------------------------------------------------------
int MysqlKit::GetRecordCount(const char* sql, int* result)
{
    if(0 == sql)    return ERROR;
    if(0 == result) return ERROR;

    if(0 != mysql_query(reinterpret_cast<MYSQL*>(mysql_), sql))
    {
        OnError(std::string("mysql_query:") + mysql_error(reinterpret_cast<MYSQL*>(mysql_)));
        return ERROR;
    }

    if(0 == (reinterpret_cast<MYSQL*>(mysql_)->field_count))
    {
        OnError(std::string("reinterpret_cast<MYSQL*>(mysql_)->field_count"));
        return ERROR;
    }

    MYSQL_RES* res = mysql_store_result(reinterpret_cast<MYSQL*>(mysql_));
    if(0 == res)
    {
        OnError(std::string("mysql_store_result:") + mysql_error(reinterpret_cast<MYSQL*>(mysql_)));
        return ERROR;
    }

    MYSQL_ROW row   = mysql_fetch_row(res);
    *result         = std::stoi(row[0]);
    mysql_free_result(res);
    return SUCCESS;
}
//---------------------------------------------------------------------------
int MysqlKit::GetRecordOne(const std::string& sql, Records* result)
{
    return GetRecordOne(sql.c_str(), result);
}
//---------------------------------------------------------------------------
int MysqlKit::GetRecordOne(const char* sql, Records* result)
{
    if(0 == sql)    return ERROR;
    if(0 == result) return ERROR;

    result->clear();

    if(0 != mysql_query(reinterpret_cast<MYSQL*>(mysql_), base::CombineString("%s LIMIT 0,1", sql).c_str()))
    {
        OnError(std::string("mysql_query:") + mysql_error(reinterpret_cast<MYSQL*>(mysql_)));
        return ERROR;
    }

    //field域必须>0
    if(0 == (reinterpret_cast<MYSQL*>(mysql_)->field_count))
    {
        OnError(std::string("query field 0"));
        return ERROR;
    }

    MYSQL_RES* res = mysql_store_result(reinterpret_cast<MYSQL*>(mysql_));
    if(0 == res)
    {
        OnError(std::string("mysql_store_result:") + mysql_error(reinterpret_cast<MYSQL*>(mysql_)));
        return ERROR;
    }

    //没有数据
    int counts = static_cast<int>(mysql_num_rows(res));
    if(0 == counts)
    {
        mysql_free_result(res);
        return SUCCESS;
    }

    //填充数据
    MYSQL_ROW row = mysql_fetch_row(res);
    for(int i = 0; i<static_cast<int>(reinterpret_cast<MYSQL*>(mysql_)->field_count); i++)
    {
        if(0 == row[i])
            result->push_back("");
        else
            result->push_back(row[i]);
    }

    mysql_free_result(res);
    return SUCCESS;
}
//---------------------------------------------------------------------------
int MysqlKit::GetRecord(const std::string& sql, RecordList* result, size_t offset, size_t size)
{
    return GetRecord(sql.c_str(), result, offset, size);
}
//---------------------------------------------------------------------------
int MysqlKit::GetRecord(const char* sql, RecordList* result, size_t offset, size_t size)
{
    if(0 == sql)            return ERROR;
    if(0 == result)         return ERROR;
    if(size > kRecordMax)   return RECORD_TOOMORE;

    result->clear();

    if(0 != mysql_query(reinterpret_cast<MYSQL*>(mysql_), base::CombineString("%s LIMIT %d, %d", sql, offset, size).c_str()))
    {
        OnError(std::string("mysql_query:") + mysql_error(reinterpret_cast<MYSQL*>(mysql_)));
        return ERROR;
    }

    //field必须>0
    if(0 == (reinterpret_cast<MYSQL*>(mysql_)->field_count))
    {
        OnError(std::string("query field 0"));
        return ERROR;
    }

    //获取结果
    MYSQL_RES* res = mysql_store_result(reinterpret_cast<MYSQL*>(mysql_));
    if(0 == res)
    {
        OnError(std::string("mysql_store_result:") + mysql_error(reinterpret_cast<MYSQL*>(mysql_)));
        return ERROR;
    }

    int counts = static_cast<int>(mysql_num_rows(res));
    if(kRecordMax < counts) //数据条数过多
    {
        mysql_free_result(res);
        return RECORD_TOOMORE;
    }

    if(0 == counts) //无数据
    {
        mysql_free_result(res);
        return SUCCESS;
    }

    result->reserve(counts);

    //填充数据
    Records record;
    for(int i = 0; i<counts; i++)
    {
        MYSQL_ROW row = mysql_fetch_row(res);
        if(0 == row)
            break;

        record.clear();
        for(int j = 0; j<static_cast<int>(reinterpret_cast<MYSQL*>(mysql_)->field_count); j++)
        {
            if(0 == row[j])
                record.push_back("");
            else
                record.push_back(row[j]);
        }

        result->push_back(record);
    }

    mysql_free_result(res);
    return SUCCESS;
}
//---------------------------------------------------------------------------
void MysqlKit::TransactionBegin()
{
    in_transaction_     = true;
    transaction_result_ = true;

    return;
}
//---------------------------------------------------------------------------
void MysqlKit::TransactionEnd()
{
    assert(true == in_transaction_);

    if(true == transaction_result_)
    {
        if(0 != mysql_commit(reinterpret_cast<MYSQL*>(mysql_)))
        {
            OnError(std::string("mysql_conmmit error:") + mysql_error(reinterpret_cast<MYSQL*>(mysql_)));
            assert(0);
            return;
        }
    }
    else
    {
        if(0 != mysql_rollback(reinterpret_cast<MYSQL*>(mysql_)))
        {
            OnError(std::string("mysql_rollback error:") + mysql_error(reinterpret_cast<MYSQL*>(mysql_)));
            return;
        }
    }

    in_transaction_     = false;
    transaction_result_ = false;
    return;
}
//---------------------------------------------------------------------------
bool MysqlKit::Reconect()
{
    if(0 == mysql_ping(reinterpret_cast<MYSQL*>(mysql_)))
        return true;

    Disconnect();

    if(false == Connect(host_, port_, user_, password_, database_, charset_))
    {
        OnError("Reconnect error");
        return false;
    }

    return true;;
}
//---------------------------------------------------------------------------
void MysqlKit::OnError(const std::string& message)
{
    if(callback_db_error_)
        callback_db_error_(message);

    return;
}
//---------------------------------------------------------------------------
}//namespace net
