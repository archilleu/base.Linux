//---------------------------------------------------------------------------
#include "json_reader.h"
#include "value.h"
#include "../function.h"
//---------------------------------------------------------------------------
namespace base
{

namespace json
{
//---------------------------------------------------------------------------
JsonReader::JsonReader()
{
}
//---------------------------------------------------------------------------
JsonReader::~JsonReader()
{
}
//---------------------------------------------------------------------------
bool JsonReader::Parse(const std::string& str, Value* root)
{
    return Parse(str.c_str(), root);
}
//---------------------------------------------------------------------------
bool JsonReader::Parse(const char* str, Value* root)
{
    token_reader_.set_dat(MemoryBlock(str, strlen(str)));
    return _Parse(root);
}
//---------------------------------------------------------------------------
bool JsonReader::Parse(MemoryBlock&& dat, Value* root)
{
    token_reader_.set_dat(std::move(dat));
    return _Parse(root);
}
//---------------------------------------------------------------------------
bool JsonReader::ParseFile(const std::string& path, Value* root)
{
    return ParseFile(path.c_str(), root);
}
//---------------------------------------------------------------------------
bool JsonReader::ParseFile(const char* path, Value* root)
{
    MemoryBlock dat;
    if(false == base::LoadFile(path, &dat))
        return false;

    token_reader_.set_dat(std::move(dat));
    return _Parse(root);
}
//---------------------------------------------------------------------------
bool JsonReader::_Parse(Value* root)
{
    //解析栈
    std::stack<Value> parse_stack;

    //初始状态
    cur_status_ = kSTATUS_OBJECT_BEGIN | kSTATUS_SINGLE_VALUE | kSTATUS_ARRAY_BEGIN;
    for(;;)
    {
        int token = token_reader_.ReadNextToken();
        switch(token)
        {
            // '\0'end
            case TokenReader::TokenType::DOCUMENT_END:
                {
                if(false == CaseStatusDocumentEnd(parse_stack, root))
                    return false;

                return true;
                }

            // {
            case TokenReader::TokenType::OBJECT_BEGIN:
                {
                if(false == CaseStatusObjectBegin(parse_stack, root))
                    return false;

                break;
                }

            // }
            case TokenReader::TokenType::OBJECT_END:
                {
                if(false == CaseStatusObjectEnd(parse_stack, root))
                    return false;

                break;
                }

            // [
            case TokenReader::TokenType::ARRAY_BEGIN:
                {
                if(false == CaseStatusArrayBegin(parse_stack, root))
                    return false;

                break;
                }

            // ]
            case TokenReader::TokenType::ARRAY_END:
                {
                if(false == CaseStatusArrayEnd(parse_stack, root))
                    return false;

                break;
                }

            // :
            case TokenReader::TokenType::SEP_COLON:
                {
                if(false == CaseStatusSepColon(parse_stack, root))
                    return false;

                break;
                }

            // ,
            case TokenReader::TokenType::SEP_COMMA:
                {
                if(false == CaseStatusSepComma(parse_stack, root))
                    return false;

                break;
                }

            // string
            case TokenReader::TokenType::STRING:
                {
                if(HasStatus(kSTATUS_OBJECT_KEY))   // key
                {
                    if(false == CaseStatusObjectKey(parse_stack, root))
                        return false;

                    break;
                }

                if(HasStatus(kSTATUS_OBJECT_VALUE)) // obj value
                {
                    if(false == CaseStatusObjectValue(parse_stack, root, Value::TYPE_STRING))
                        return false;

                    break;
                }

                if(HasStatus(kSTATUS_ARRAY_VALUE))  // array value
                {
                    if(false == CaseStatusArrayValue(parse_stack, root, Value::TYPE_STRING))
                        return false;

                    break;
                }

                assert(0);
                break;
                }

            //t or f
            case TokenReader::TokenType::BOOLEAN:
                
                if(HasStatus(kSTATUS_OBJECT_VALUE)) // obj value
                {
                    if(false == CaseStatusObjectValue(parse_stack, root, Value::TYPE_BOOLEAN))
                        return false;

                    break;
                }

                if(HasStatus(kSTATUS_ARRAY_VALUE))  // array value
                {
                    if(false == CaseStatusArrayValue(parse_stack, root, Value::TYPE_BOOLEAN))
                        return false;

                    break;
                }
                
                break;

            //- or [0-9]
            case TokenReader::TokenType::NUMBER:

                if(HasStatus(kSTATUS_OBJECT_VALUE)) // obj value
                {
                    if(false == CaseStatusObjectValue(parse_stack, root, Value::TYPE_NUMBER))
                        return false;

                    break;
                }

                if(HasStatus(kSTATUS_ARRAY_VALUE))  // array value
                {
                    if(false == CaseStatusArrayValue(parse_stack, root, Value::TYPE_NUMBER))
                        return false;

                    break;
                }

                break;

            //null
            case TokenReader::TokenType::NUL:

                if(HasStatus(kSTATUS_OBJECT_VALUE)) // obj value
                {
                    if(false == CaseStatusObjectValue(parse_stack, root, Value::TYPE_NULL))
                        return false;

                    break;
                }

                if(HasStatus(kSTATUS_ARRAY_VALUE))  // array value
                {
                    if(false == CaseStatusArrayValue(parse_stack, root, Value::TYPE_NULL))
                        return false;

                    break;
                }

                break;

            //
            case TokenReader::TokenType::INVALID:
                break;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool JsonReader::CaseStatusObjectBegin(std::stack<Value>& parse_stack, Value* root)
{
    if(!HasStatus(kSTATUS_OBJECT_BEGIN))
        return false;

    root->set_type(Value::TYPE_OBJECT);
    //{ -> "key" or { or }
    parse_stack.push(*root);
    cur_status_ = kSTATUS_OBJECT_KEY | kSTATUS_OBJECT_BEGIN | kSTATUS_OBJECT_END;
    return true;
}
//---------------------------------------------------------------------------
bool JsonReader::CaseStatusObjectKey(std::stack<Value>& parse_stack, Value*)
{
    std::string key;
    bool err_code = token_reader_.ReadString(key);
    if(false == err_code)
        return false;
    
    Value value(Value::TYPE_KEY);
    value.set_str(std::move(key));
    parse_stack.push(std::move(value));

    //"key" - > :
    cur_status_ = kSTATUS_SEP_COLON;
    return true;
}
//---------------------------------------------------------------------------
bool JsonReader::CaseStatusObjectValue(std::stack<Value>& parse_stack, Value*, int type)
{
    //key:value,parse_stack.size()>=2
    if(2 > parse_stack.size())
        return false;

    //key
    if(Value::TYPE_KEY != parse_stack.top().type())
        return false;
    std::string key = parse_stack.top().val();
    parse_stack.pop();

    //value
    Value       value;
    std::string str;
    switch(type)
    {
        case Value::TYPE_STRING:
            {
            if(false == token_reader_.ReadString(str))
                return false;

            value.set_type(Value::TYPE_STRING);
            value.set_str(std::move(str));

            break;
            }

        case Value::TYPE_NUMBER:
            {
            Value::ValueType num_type;
            if(false == token_reader_.ReadNumber(str, num_type))
                return false;
            value.set_number(str, num_type);

            break;
            }

        case Value::TYPE_BOOLEAN:
            {
            bool boolean;
            if(false == token_reader_.ReadBoolean(boolean))
                return false;

            value.set_type(Value::TYPE_BOOLEAN);
            value.set_boolean(boolean);
            
            break;
            }

        case Value::TYPE_NULL:
            {
            if(false == token_reader_.ReadNull())
                return false;
            value.set_type(Value::TYPE_NULL);

            break;
            }

        default:
            break;
    }

    //插入
    Value& object = parse_stack.top();
    object.PairAdd(std::move(key), std::move(value));

    //"value" -> , or }
    cur_status_ = kSTATUS_SEP_COMMA | kSTATUS_OBJECT_END;
    return true;
}
//---------------------------------------------------------------------------
bool JsonReader::CaseStatusObjectEnd(std::stack<Value>& parse_stack, Value*)
{
    if(!HasStatus(kSTATUS_OBJECT_END))
        return false;

    //栈底元素类型必须是object
    if(Value::TYPE_OBJECT != parse_stack.top().type())
        return false;
    
    //如果是唯一元素,则JSON解析结束
    if(1 == parse_stack.size())
        cur_status_ = kSTATUS_DOCUMENT_END;
    else
        cur_status_ = kSTATUS_SEP_COLON;
    
    return true;
}
//---------------------------------------------------------------------------
bool JsonReader::CaseStatusArrayBegin(std::stack<Value>& parse_stack, Value*)
{
    if(!HasStatus(kSTATUS_ARRAY_BEGIN))
        return false;

    //添加array
    parse_stack.push(Value(Value::TYPE_ARRAY));

    //[ -> " or [0-9] or { or [ or ]
    cur_status_ = kSTATUS_ARRAY_VALUE | kSTATUS_ARRAY_BEGIN | kSTATUS_ARRAY_END | kSTATUS_OBJECT_BEGIN;
    return true;
}
//---------------------------------------------------------------------------
bool JsonReader::CaseStatusArrayValue(std::stack<Value>& parse_stack, Value*, int type)
{
    //key:[array], parse_stack.size()>=3
    if(3 > parse_stack.size())
        return false;
    
    //value
    Value       value;
    std::string str;
    switch(type)
    {
        case Value::TYPE_STRING:
            {
            if(false == token_reader_.ReadString(str))
                return false;

            value.set_type(Value::TYPE_STRING);
            value.set_str(std::move(str));

            break;
            }

        case Value::TYPE_NUMBER:
            {
            Value::ValueType num_type;
            if(false == token_reader_.ReadNumber(str, num_type))
                return false;
            value.set_number(str, num_type);

            break;
            }

        case Value::TYPE_BOOLEAN:
            {
            bool boolean;
            if(false == token_reader_.ReadBoolean(boolean))
                return false;

            value.set_type(Value::TYPE_BOOLEAN);
            value.set_boolean(boolean);
            
            break;
            }

        case Value::TYPE_NULL:
            {
            if(false == token_reader_.ReadNull())
                return false;
            value.set_type(Value::TYPE_NULL);

            break;
            }

        default:
            break;
    }

    //插入
    Value& object = parse_stack.top();
    object.ArrayAdd(std::move(value));

    //"value" -> , or ]
    cur_status_ = kSTATUS_SEP_COMMA | kSTATUS_ARRAY_END;
    return true;
}
//---------------------------------------------------------------------------
bool JsonReader::CaseStatusArrayEnd(std::stack<Value>& parse_stack, Value*)
{
    if(false == HasStatus(kSTATUS_ARRAY_END))
        return false;

    //key:[array] parse_stack.size()>=3
    if(3 > parse_stack.size())
        return false;

    //array
    Value array = parse_stack.top();
    parse_stack.pop();

    //key
    std::string key = parse_stack.top().val();
    parse_stack.pop();

    //key:[array]添加到object中
    parse_stack.top().PairAdd(std::move(key), std::move(array));

    //] -> , or }
    cur_status_ = kSTATUS_SEP_COMMA | kSTATUS_OBJECT_END;
    return true;
}
//---------------------------------------------------------------------------
bool JsonReader::CaseStatusSepColon(std::stack<Value>&, Value*)
{
    if(!HasStatus(kSTATUS_SEP_COLON))
        return false;

    //: -> { or [ or "value"
    cur_status_ = kSTATUS_OBJECT_BEGIN | kSTATUS_ARRAY_BEGIN | kSTATUS_OBJECT_VALUE;
    return true;
}
//---------------------------------------------------------------------------
bool JsonReader::CaseStatusSepComma(std::stack<Value>&, Value*)
{
    if(!HasStatus(kSTATUS_SEP_COMMA))
        return false;

    ///, -> "key" or [ or { or arr_val
    //如果当前状态同时期待kSTATUS_OBJECT_END,代表当前处于key:value状态
    if(HasStatus(kSTATUS_OBJECT_END))
    {
        cur_status_ = kSTATUS_OBJECT_KEY;
        return true;
    }

    //如果当前状态同时期待kSTATUS_ARRAY_END,代表当前处于key:[array]状态
    if(HasStatus(kSTATUS_ARRAY_END))
    {
        cur_status_ = kSTATUS_OBJECT_BEGIN | kSTATUS_ARRAY_BEGIN | kSTATUS_ARRAY_VALUE;
        return true;
    }

    return true;
}
//---------------------------------------------------------------------------
bool JsonReader::CaseStatusSignalValue(std::stack<Value>& , Value* , int )
{
    return true;
}
//---------------------------------------------------------------------------
bool JsonReader::CaseStatusDocumentEnd(std::stack<Value>& parse_stack, Value* root)
{
    if(!HasStatus(kSTATUS_DOCUMENT_END))
        return false;

    assert(1 == parse_stack.size());

    *root = parse_stack.top();
    parse_stack.pop();

    return true;
}
//---------------------------------------------------------------------------
}//namespace json

}//namespace base
