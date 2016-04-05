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
    int         deep = 0;
    ToString(value, str, format, deep); 

    return str;
}
//---------------------------------------------------------------------------
void JsonWriter::ToString(const Value& value, std::string& str, bool format, int deep)
{
    int type = value.type();
    if(Value::TYPE_OBJECT == type)
    {
        ObjectToString(value, str, format, deep);
    }

    if(Value::TYPE_ARRAY == type)
    {
        ArrayToString(value, str, format, deep);
    }

    return;
}
//---------------------------------------------------------------------------
void JsonWriter::ObjectToString(const Value& value, std::string& str, bool format, int deep)
{
    if(format)  AddTab(str, deep);
        str.push_back('{');
    if(format)  str.push_back('\n');
    if(format)  deep++;

    for(auto iter=value.PairIterBegin(); iter!=value.PairIterEnd(); ++iter)
    {
        //添加key
        str.push_back('\"');
            str += iter->first;
        str += "\":";

        //添加value
        switch(iter->second.type())
        {
            case Value::TYPE_OBJECT:
            case Value::TYPE_ARRAY:
                if(format)
                    str.push_back('\n');
                ToString(iter->second, str, format, deep+1);
                break;

            case Value::TYPE_STRING:
                str.push_back('\"');
                    str += iter->second.val();
                str.push_back('\"');
                break;

            case Value::TYPE_INT:
            case Value::TYPE_UINT:
            case Value::TYPE_REAL:
            case Value::TYPE_BOOLEAN:
            case Value::TYPE_NULL:
            case Value::TYPE_KEY:
                if(format)
                    AddTab(str, deep);
                str += iter->second.val();
                break;

            default:
                assert(0);
         }

        str.push_back(',');
        if(format)
            str.push_back('\n');
    }
    if(format)              if('\n' == str.back())  str.pop_back();
    if(',' == str.back())   str.pop_back();

    if(format)
    {
        str.push_back('\n');
        AddTab(str, deep--);
    }
    str.push_back('}');

    return;
}
//---------------------------------------------------------------------------
void JsonWriter::ArrayToString(const Value& value, std::string& str, bool format, int deep)
{
    if(format)  AddTab(str, deep);
        str.push_back('[');
    if(format)  str.push_back('\n');
    if(format)  deep ++;

    for(auto iter=value.ArrayIterBegin(); value.ArrayIterEnd()!=iter; ++iter)
    {
        //添加value
        switch(iter->type())
        {
            case Value::TYPE_OBJECT:
            case Value::TYPE_ARRAY:
                if(format)
                    str.push_back('\n');
                ToString(*iter, str, format, deep+1);
                break;

            case Value::TYPE_STRING:
                str.push_back('\"');
                    str += iter->val();
                str.push_back('\"');
                break;

            case Value::TYPE_INT:
            case Value::TYPE_UINT:
            case Value::TYPE_REAL:
            case Value::TYPE_BOOLEAN:
            case Value::TYPE_NULL:
            case Value::TYPE_KEY:
                if(format)
                    AddTab(str, deep);
                str += iter->val();
                break;

            default:
                assert(0);
        }
        
        str.push_back(',');
        if(format)
            str.push_back('\n');
    }
    if(format)              if('\n' == str.back())  str.pop_back();
    if(',' == str.back())   str.pop_back();

    if(format)
    {
        str.push_back('\n');
        AddTab(str, deep--);
    }
    str.push_back(']');

    return;
}
//---------------------------------------------------------------------------
}//namespace json

}//namespace base
