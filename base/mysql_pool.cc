//---------------------------------------------------------------------------
#include "mysql_pool.h"
#include "mysql_kit.h"
//---------------------------------------------------------------------------
namespace base
{
//---------------------------------------------------------------------------
MysqlPool::MysqlPool(const std::string& host, short port, const std::string& user, const std::string& password, const std::string& database, const std::string& charset)
:   host_(host),
    port_(port),
    user_(user),
    password_(password),
    database_(database),
    charset_(charset)
{
}
//---------------------------------------------------------------------------
MysqlPool::~MysqlPool()
{
    std::lock_guard<std::mutex> lock(mutex_);

    while(!obj_list_.empty())
    {
        obj_list_.top()->Disconnect();
        obj_list_.pop();
    }

    return;
}
//---------------------------------------------------------------------------
std::shared_ptr<MysqlKit> MysqlPool::Get()
{
    std::lock_guard<std::mutex> lock(mutex_);

    if(obj_list_.empty())
        return ObjectCreate();

    std::shared_ptr<MysqlKit> obj = obj_list_.top();
    obj_list_.pop();
    return obj;
}
//---------------------------------------------------------------------------
void MysqlPool::Put(const std::shared_ptr<MysqlKit>& obj)
{
    std::lock_guard<std::mutex> lock(mutex_);

    obj_list_.push(obj);
    return;
}
//---------------------------------------------------------------------------
std::shared_ptr<MysqlKit> MysqlPool::ObjectCreate()
{
    std::shared_ptr<MysqlKit> obj = std::make_shared<MysqlKit>();
    obj->Set_callback_db_error(callback_db_error_);

    if(false == obj->Connect(host_, port_, user_, password_, database_, charset_))
    {
        obj.reset();
    }

    return obj;
}
//---------------------------------------------------------------------------
}//namespace base
