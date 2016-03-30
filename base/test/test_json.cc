//---------------------------------------------------------------------------
#include "test_json.h"
#include "../json/token_reader.h"
#include "../memory_block.h"
//---------------------------------------------------------------------------
using namespace base;
using namespace base::test;
//---------------------------------------------------------------------------
bool TestJson::DoTest()
{
    if(false == Test_Value())       return false;
    if(false == Test_CharReader())  return false;
    if(false == Test_TokenReader()) return false;
    if(false == Test_Normal())      return false;

    return true;
}
//---------------------------------------------------------------------------
bool TestJson::Test_Value()
{
    return true;
}
//---------------------------------------------------------------------------
bool TestJson::Test_CharReader()
{
    json::CharReader char_reader;
    std::string str =   "{               \
                            \"a\":\"b\"     \
                         }";
    MemoryBlock mb(str.data(), str.length());
    char_reader.set_dat(std::move(mb));

    for(char c=char_reader.Peek(); true==char_reader.HasMore(); c=char_reader.Next())
    {
        std::cout << c;
    }
    std::cout << std::endl;

    return true;
}
//---------------------------------------------------------------------------
bool TestJson::Test_TokenReader()
{
    return true;
}
//---------------------------------------------------------------------------
bool TestJson::Test_Normal()
{
    json::TokenReader reader;
    std::string str =   "{               \
                            \"a\":\"b\"     \
                         }";
    MemoryBlock mb(str.data(), str.length());
    reader.set_dat(std::move(mb));
    
    int type = reader.ReadNextToken();
    if(json::TokenReader::TokenType::OBJECT_BEGIN == type)
    {
        std::string key;
        bool err_code = reader.ReadString(key);
        assert(true == err_code);
    }
    return true;
}
//---------------------------------------------------------------------------
