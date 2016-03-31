//---------------------------------------------------------------------------
#ifndef LINUX_BASE_JSON_TOKEN_READER_H_
#define LINUX_BASE_JSON_TOKEN_READER_H_
//---------------------------------------------------------------------------
#include "../share_inc.h"
#include "char_reader.h"
#include "../memory_block.h"
//---------------------------------------------------------------------------
namespace base
{

namespace json
{

class TokenReader
{
public:
    TokenReader()
    {
    }
    ~TokenReader()
    {
    }

    //设置要分析的数据
    void set_dat(MemoryBlock&& dat) { char_reader_.set_dat(std::move(dat)); }

    //token类型
    enum TokenType
    {
        DOCUMENT_END = 1,   // 分析结束
        OBJECT_BEGIN,       // obj开始
        OBJECT_END,         // obj结束
        ARRAY_BEGIN,        // array开始
        ARRAY_END,          // array结束
        SEP_COLON,          // 分隔符 :
        SEP_COMMA,          // 分隔符 ,
        STRING,             // 字符串
        BOOLEAN,            // 布尔值
        NUMBER,             // 数值
        NUL,                // 空
        INVALID             // 非法的字符
    };

    //读取下一个Token
    TokenType ReadNextToken();
    
    //读取字符串等值
    bool ReadString(std::string& str);

    //数值类型
    struct Number
    {
        union
        {
            int64_t     u_int;
            uint64_t    u_uint;
            double      u_double;
            
        }value_;

        short type_;
        enum
        {
            TYPE_INT = 1,
            TYPE_UINT,
            TYPE_DOUBLE,
            TYPE_UDOUBLE
        };

        void set_value(int64_t value)   { type_ = TYPE_INT;     value_.u_int    = value; }
        void set_value(uint64_t value)  { type_ = TYPE_UINT;    value_.u_uint   = value; }
        void set_value(double value)    { type_ = TYPE_DOUBLE;  value_.u_double = value; }

        int64_t     get_int()       { assert(TYPE_INT       == type_);  return value_.u_int; }
        uint64_t    get_uint()      { assert(TYPE_UINT      == type_);  return value_.u_uint; }
        double      get_double()    { assert(TYPE_DOUBLE    == type_);  return value_.u_double; }
    };
    //读取数值类型
    bool ReadNumber(Number& number);

    //布尔类型
    bool ReadBoolean(bool& boolean);

    //空类型
    bool ReadNull();

private:
    //判断是否空白字符
    bool IsWhitespace(char c)
    {
        return ((' '==c) || ('\t'==c) || ('\n'==c) || ('\r'==c));
    }

    void SkipWhitespace();

    bool CheckNumber(char c)
    {
        if((isdigit(c)) || ('.'==c) || ('e'==c) || ('E'==c))
            return true;

        return false;
    }

private:
    CharReader char_reader_;
};

}//namespace json

}//namespace base
//---------------------------------------------------------------------------
#endif //LINUX_BASE_JSON_TOKEN_READER_H_
