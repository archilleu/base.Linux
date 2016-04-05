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

    static std::string ToString(const Value& value, bool format=false);

private:
    static void ToString        (const Value& value, std::string& str, bool format, int deep);
    static void ObjectToString  (const Value& value, std::string& str, bool format, int deep);
    static void ArrayToString   (const Value& value, std::string& str, bool format, int deep);

    static void AddTab(std::string& str, int deep)
    {
        for(int i=0; i<deep; i++)
            str.push_back('\t');
    }

    static void AddString(const std::string& from, std::string& str);
};

}//namespace json

}//namespace base
//---------------------------------------------------------------------------
#endif //LINUX_BASE_JSON_JSON_WRITER_H_
