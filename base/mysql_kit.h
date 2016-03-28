//---------------------------------------------------------------------------
#ifndef LINUX_BASE_MYSQL_KIT_H_
#define LINUX_BASE_MYSQL_KIT_H_
//---------------------------------------------------------------------------
#include "share_inc.h"
//---------------------------------------------------------------------------
namespace base
{

class MysqlKit
{
public:
    MysqlKit(void);
    ~MysqlKit(void);

    enum
    {
        SUCCESS = 1,
        RECORD_TOOMORE,
        ERROR
    };

    typedef std::vector<std::string>    Records;
    typedef std::vector<Records>        RecordList;

    typedef std::function<void (const std::string&)> CallbackDBError;

public:
    void  Set_callback_db_error(const CallbackDBError& callback)  { callback_db_error_ = callback; }

    bool Connect    (const std::string& host, short port, const std::string& user, const std::string& password, const std::string& database, const std::string& charset="utf8");
    void Disconnect ();

    int Execute(const std::string& sql, int* err_no=0, int* affected_rows=0, uint64_t* last_insert_id=0);
    int Execute(const char* sql, int* err_no=0, int* affected_rows=0, uint64_t* last_insert_id=0);

    int GetRecordCount(const std::string& sql, int* count_ret);
    int GetRecordCount(const char* sql, int* count_ret);

    int GetRecordOne(const std::string& sql, Records* result);
    int GetRecordOne(const char* sql, Records* result);
    int GetRecord   (const std::string& sql, RecordList* result, size_t offset=0, size_t size=8192);
    int GetRecord   (const char* sql, RecordList* result, size_t offset=0, size_t size=8192);

    bool SetRecordValue(const std::string& table, const std::string& field, const std::string& where, char* value, int len);    //设定指定位置的值(可用于二进制数据)
    bool GetRecordValue(const std::string& table, const std::string& field, const std::string& where, std::string* value);      //获取指定位置的值(可用于二进制数据)

    void TransactionBegin();
    void TransactionEnd();

public:
  const static int kRecordMax;
  const static int kDupEntry;
  const static int kNoReferenced;

private:
  bool Reconect();

  void OnError(const std::string& message);

private:
  short port_;
  std::string host_;
  std::string user_;
  std::string password_;
  std::string database_;
  std::string charset_;

  bool in_transaction_;
  bool transaction_result_;

  void* mysql_;

  CallbackDBError callback_db_error_;

protected:
    DISALLOW_COPY_AND_ASSIGN(MysqlKit);
};

} //namespace net
//---------------------------------------------------------------------------
#endif //LINUX_BASE_MYSQL_KIT_H_

