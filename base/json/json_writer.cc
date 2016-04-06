//---------------------------------------------------------------------------
#include "json_writer.h"
#include "value.h"
#include "../function.h"
//---------------------------------------------------------------------------
namespace base
{

namespace json
{
//---------------------------------------------------------------------------
std::string JsonWriter::ToString(const Value& value, bool format)
{
    std::string str;
    int         deep = 1;
    ToString(value, str, format, deep); 

    return str;
}
//---------------------------------------------------------------------------
void JsonWriter::ToString(const Value& value, std::string& str, bool format, int deep)
{
    int type = value.type();
    switch(type)
    {
        case Value::TYPE_OBJECT:
            ObjectToString(value, str, format, deep);
            break;

        case Value::TYPE_ARRAY:
            ArrayToString(value, str, format, deep);
            break;

        case Value::TYPE_STRING:
        case Value::TYPE_KEY:
            str.push_back('\"');
                str += value.val();
            str.push_back('\"');

            break;

        case Value::TYPE_INT:
        case Value::TYPE_UINT:
        case Value::TYPE_REAL:
        case Value::TYPE_BOOLEAN:
        case Value::TYPE_NULL:
            str += value.val();
            break;

        default:
            assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
void JsonWriter::ObjectToString(const Value& value, std::string& str, bool format, int deep)
{
    str.push_back('{');
    if(format)  str.push_back('\n');

    for(auto iter=value.PairIterBegin(); iter!=value.PairIterEnd(); ++iter)
    {
        if(format)  AddTab(str, deep);

        //添加key
        str.push_back('\"');
            str += iter->first;
        str += "\":";

        //添加value
        switch(iter->second.type())
        {
            case Value::TYPE_OBJECT:
            case Value::TYPE_ARRAY:
                ToString(iter->second, str, format, deep+1);
                break;

            case Value::TYPE_STRING:
                str.push_back('\"');
                    AddString(iter->second.val(), str);
                str.push_back('\"');
                break;

            case Value::TYPE_INT:
            case Value::TYPE_UINT:
            case Value::TYPE_REAL:
            case Value::TYPE_BOOLEAN:
            case Value::TYPE_NULL:
            case Value::TYPE_KEY:
                str += iter->second.val();
                break;

            default:
                assert(0);
         }

        str.push_back(',');
        if(format)  str.push_back('\n');
    }
    if(format)              if('\n' == str.back())  str.pop_back();
    if(',' == str.back())   str.pop_back();

    if(format)  { str.push_back('\n'); AddTab(str, deep-1); }
    str.push_back('}');

    return;
}
//---------------------------------------------------------------------------
void JsonWriter::ArrayToString(const Value& value, std::string& str, bool format, int deep)
{
    str.push_back('[');
    if(format)  str.push_back('\n');

    for(auto iter=value.ArrayIterBegin(); value.ArrayIterEnd()!=iter; ++iter)
    {
        if(format) AddTab(str, deep);

        //添加value
        switch(iter->type())
        {
            case Value::TYPE_OBJECT:
            case Value::TYPE_ARRAY:
                ToString(*iter, str, format, deep+1);
                break;

            case Value::TYPE_STRING:
                str.push_back('\"');
                    AddString(iter->val(), str);
                str.push_back('\"');
                break;

            case Value::TYPE_INT:
            case Value::TYPE_UINT:
            case Value::TYPE_REAL:
            case Value::TYPE_BOOLEAN:
            case Value::TYPE_NULL:
            case Value::TYPE_KEY:
                str += iter->val();
                break;

            default:
                assert(0);
        }
        
        str.push_back(',');
        if(format)  str.push_back('\n');
    }
    if(format)              if('\n' == str.back())  str.pop_back();
    if(',' == str.back())   str.pop_back();

    if(format) { str.push_back('\n'); AddTab(str, deep-1); }
    str.push_back(']');

    return;
}
//---------------------------------------------------------------------------
void JsonWriter::AddString(const std::string& from, std::string& str)
{
    for(auto iter=from.begin(); from.end()!=iter; ++iter)
    {
        switch(*iter)
        {
            case '\"':
                str += "\\\"";
                break;

            case '\\':
                str += "\\\\";
                break;

            case '/':
                str += "\\/";
                break;

            case '\b':
                str += "\\b";
                break;

            case '\f'://换页
                str += "\\f";
                break;

            case '\n':
                str += "\\n";
                break;

            case '\r':
                str += "\\r";
                break;

            case '\t':
                str += "\\t";
                break;

            //case 'u'://unicode字符
                //todo
                break;

            default:
                str.push_back(*iter); 
                break;
        }
    }

    return;
}
//---------------------------------------------------------------------------
}//namespace json

}//namespace base
