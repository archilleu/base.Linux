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
bool JsonReader::_Parse(Value* )
{
    //解析JSON栈
    std::stack<Value> parse_stack;

    cur_status_ = kSTATUS_OBJECT_BEGIN | kSTATUS_SINGLE_VALUE | kSTATUS_ARRAY_BEGIN;
    for(;;)
    {
        int token = token_reader_.ReadNextToken();
        switch(token)
        {
            case TokenReader::TokenType::DOCUMENT_END:  // '\0'end
                {
                if(!HasStatus(kSTATUS_DOCUMENT_END))
                    return false;

                assert(1 == parse_stack.size());

                Value root = parse_stack.top();
                parse_stack.pop();
                return true;
                }

                break;

            case TokenReader::TokenType::OBJECT_BEGIN:
                {
                if(!HasStatus(kSTATUS_OBJECT_BEGIN))
                    return false;

                //{ -> "key" or { or }
                parse_stack.push(Value(Value::TYPE_OBJECT));
                cur_status_ = kSTATUS_OBJECT_KEY | kSTATUS_OBJECT_BEGIN | kSTATUS_OBJECT_END;
                }

                break;

            case TokenReader::TokenType::OBJECT_END:    // }
                {
                if(!HasStatus(kSTATUS_OBJECT_END))
                    return false;

                //栈底元素类型必须是object
                if(Value::TYPE_OBJECT != parse_stack.top().type())
                    return false;
                
                //如果是唯一元素,则JSON解析结束
                if(1 == parse_stack.size())
                {
                    cur_status_ = kSTATUS_DOCUMENT_END;
                    break;
                }
                }

                break;

            case TokenReader::TokenType::ARRAY_BEGIN:
                break;

            case TokenReader::TokenType::ARRAY_END:
                break;

            case TokenReader::TokenType::SEP_COLON:     // :
                {
                if(!HasStatus(kSTATUS_SEP_COLON))
                    return false;

                //: -> { or [ or "value"
                cur_status_ = kSTATUS_OBJECT_BEGIN | kSTATUS_ARRAY_BEGIN | kSTATUS_OBJECT_VALUE;
                }

                break;

            case TokenReader::TokenType::SEP_COMMA:     // ,
                {
                if(!HasStatus(kSTATUS_SEP_COMMA))
                    return false;

                //, -> "key" or { or arr_val
                cur_status_ = kSTATUS_OBJECT_KEY | kSTATUS_OBJECT_BEGIN | kSTATUS_ARRAY_VALUE;
                }

                break;

            case TokenReader::TokenType::STRING:        // string
                {
                if(HasStatus(kSTATUS_OBJECT_KEY))   // key
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
                    break;
                }

                if(HasStatus(kSTATUS_OBJECT_VALUE)) // value
                {
                    //key:value,parse_stack.size()的大小必须>=2
                    if(2 > parse_stack.size())
                        return false;

                    //key
                    if(Value::TYPE_KEY != parse_stack.top().type())
                        return false;
                    std::string key = parse_stack.top().get_str();
                    parse_stack.pop();

                    //value
                    std::string str;
                    bool err_code = token_reader_.ReadString(str);
                    if(false == err_code)
                        return false;

                    Value& object = parse_stack.top();
                    Value value(Value::TYPE_STRING);
                    value.set_str(std::move(str));
                    object.PairAdd(key, std::move(value));

                    //"value" -> , or }
                    cur_status_ = kSTATUS_SEP_COMMA | kSTATUS_OBJECT_END;
                    break;
                }

                if(HasStatus(kSTATUS_ARRAY_VALUE))
                {
                    break;
                }
                }

                break;

            case TokenReader::TokenType::BOOLEAN:
                break;

            case TokenReader::TokenType::NUMBER:
                break;

            case TokenReader::TokenType::NUL:
                break;

            case TokenReader::TokenType::INVALID:
                break;
        }
    }
    return true;
}
//---------------------------------------------------------------------------
}//namespace json

}//namespace base
