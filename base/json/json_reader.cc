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
    (void)root;
    int status  = kSTATUS_OBJECT_BEGIN | kSTATUS_SINGLE_VALUE | kSTATUS_ARRAY_BEGIN;
    int token   = token_reader_.ReadNextToken();
    for(;;)
    {
        switch(token)
        {
            case TokenReader::TokenType::DOCUMENT_END:

                break;

            case TokenReader::TokenType::OBJECT_BEGIN:
                if(!HasStatus(kSTATUS_OBJECT_BEGIN))
                    return false;

                //{ -> "key" or { or }
                status = kSTATUS_OBJECT_KEY | kSTATUS_OBJECT_BEGIN | kSTATUS_OBJECT_END;
                break;

            case TokenReader::TokenType::OBJECT_END:
                break;

            case TokenReader::TokenType::ARRAY_BEGIN:
                break;

            case TokenReader::TokenType::ARRAY_END:
                break;

            case TokenReader::TokenType::SEP_COLON:
                break;

            case TokenReader::TokenType::SEP_COMMA:
                break;

            case TokenReader::TokenType::STRING:
                if(HasStatus(kSTATUS_OBJECT_KEY))
                {
                }

                if(HasStatus(kSTATUS_OBJECT_VALUE))
                {
                }

                if(HasStatus(kSTATUS_ARRAY_VALUE))
                {
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
