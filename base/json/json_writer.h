//---------------------------------------------------------------------------
#ifndef LINUX_BASE_JSON_JSON_WRITER_H_
#define LINUX_BASE_JSON_JSON_WRITER_H_
//---------------------------------------------------------------------------
#include "../share_inc.h"
//---------------------------------------------------------------------------
namespace base
{

namespace json
{

class Value;

class JsonWriter
{
public:
    JsonWriter()
    {
    }
    ~JsonWriter()
    {
    }

    static std::string ToString(const Value& value);
private:
    static std::string _ToString(const Value& value);
};

}//namespace json

}//namespace base
//---------------------------------------------------------------------------
#endif //LINUX_BASE_JSON_JSON_WRITER_H_
