//---------------------------------------------------------------------------
#ifndef LINUX_BASE_JSON_VALUE_H_
#define LINUX_BASE_JSON_VALUE_H_
//---------------------------------------------------------------------------
#include "../share_inc.h"
//---------------------------------------------------------------------------
namespace base
{

namespace json
{

class Value
{
public:
    //Value的类型
    enum ValueType
    {
        TYPE_OBJECT_KEY = 1,    //Key类型
        TYPE_OBJECT,            //对象类型
        TYPE_ARRAY,             //数组类型
        TYPE_SINGLE,            //单件类型
        TYPE_NULL,              //空对象
        TYPE_INVALID            //无效对象
    };
    Value(ValueType type);
    ~Value();
    
public:
    void set_int    (int64_t value)     { base_value_.u_int     = value; }
    void set_uint   (uint64_t value)    { base_value_.u_uint    = value; }
    void set_boolean(bool value)        { base_value_.u_real    = value; }
    void set_double (double value)      { base_value_.u_bool    = value; }

    void set_str(const char* value)         { str_ = value; }
    void set_str(const std::string& value)  { str_ = value; }
    void set_str(std::string&& value)       { str_ = std::move(value); }

private:
    std::string key_;
    ValueType   type_;

    //部分value的数据,有效内容由type_字段指出
    union
    {
        int64_t     u_int;
        uint64_t    u_uint;
        double      u_real;
        bool        u_bool;
    }base_value_;
    std::string                     str_;
    std::vector<Value>              array_;
    std::map<std::string, Value>    objects_;
};

}//namespace json

}//namespace base
//---------------------------------------------------------------------------
#endif //LINUX_BASE_JSON_VALUE_H_
