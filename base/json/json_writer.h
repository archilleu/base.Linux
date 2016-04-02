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
    static void ToString        (const Value& value, std::string& str);
    static void ObjectToString  (const Value& value, std::string& str);
    static void ArrayToString   (const Value& value, std::string& str);
    static void NumberToString  (const Value& value, std::string& str);
    static void BooleanToString (const Value& value, std::string& str);
    static void NullToString    (const Value& value, std::string& str);
};

}//namespace json

}//namespace base
//---------------------------------------------------------------------------
#endif //LINUX_BASE_JSON_JSON_WRITER_H_
