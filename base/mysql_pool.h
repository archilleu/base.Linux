//---------------------------------------------------------------------------
#ifndef LINUX_BASE_MYSQL_POOL_H_
#define LINUX_BASE_MYSQL_POOL_H_
//---------------------------------------------------------------------------
#include "share_inc.h"
#include "mysql_kit.h"
//---------------------------------------------------------------------------
namespace base
{

class MysqlKit;

class MysqlPool
{
public:
    MysqlPool(const std::string& host, short port, const std::string& user, const std::string& password, const std::string& database, const std::string& charset="utf8");
    ~MysqlPool();

    void set_callback_db_error(const MysqlKit::CallbackDBError& callback)   { callback_db_error_ = callback; }

    std::shared_ptr<MysqlKit>   Get();
    void                        Put(const std::shared_ptr<MysqlKit>& obj);

private:
    std::shared_ptr<MysqlKit>   ObjectCreate();

private:
    std::stack<std::shared_ptr<MysqlKit>>   obj_list_;
    std::mutex                              mutex_;

    std::string host_;
    short       port_;
    std::string user_;
    std::string password_;
    std::string database_;
    std::string charset_;

    MysqlKit::CallbackDBError callback_db_error_;

protected:
    DISALLOW_COPY_AND_ASSIGN(MysqlPool);
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class MysqlHelper
{
public:
    MysqlHelper()
    {
        object_ = pool_->Get();
    }
    ~MysqlHelper()
    {
        pool_->Put(object_);
    }

    const std::shared_ptr<MysqlKit>& object();

    static void set_pool_(MysqlPool* pool)
    {
        pool_ = pool;
    }

private:
    std::shared_ptr<MysqlKit> object_;

private:
    static MysqlPool* pool_;
};

}//namespace base
//---------------------------------------------------------------------------
#endif //LINUX_BASE_MYSQL_POOL_H_
