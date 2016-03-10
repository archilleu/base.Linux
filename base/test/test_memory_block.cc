//---------------------------------------------------------------------------
#include "test_memory_block.h"
#include "../memory_block.h"
//---------------------------------------------------------------------------
using namespace base;
using namespace base::test;
//---------------------------------------------------------------------------
bool TestMemoryBlock::DoTest()
{
    if(false == Illegal())
    {
        assert(0);
        return false;
    }

    if(false == Legal())
    {
        assert(0);
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool TestMemoryBlock::Illegal()
{
    //no memory alloc
    {
    MemoryBlock mb(0);
    MemoryBlock mb_assig = mb;
    MemoryBlock mb_copy(mb);
    MemoryBlock mb_move(std::move(mb));
    MemoryBlock mb_move_assig = std::move(mb_move);
    }


    {
    MemoryBlock mb(0);
    MY_ASSERT(0 == mb.dat());
    assert(0 == mb.len());

    //const char* str = "hello";
    //size_t      len = strlen(str);
    //memcpy(mb.dat(), str, len); dump sgev
    }

    return true;
}
//---------------------------------------------------------------------------
bool TestMemoryBlock::Legal()
{
    {
    MemoryBlock mb(8);
    MemoryBlock mb_assig = mb;
    MemoryBlock mb_copy(mb);
    MemoryBlock mb_move(std::move(mb));
    MemoryBlock mb_move_assig = std::move(mb_move);
   
    assert(8 == mb_copy.len());

    const char* str = "hello";
    size_t      len = strlen(str) + 1;
    memcpy(mb_copy.dat(), str, len);

    assert(0 == strcmp(mb_copy.dat(), str));
    }

    {
    MemoryBlock mb(base::UNIT_MB*200);
    mb.Fill('a');
    MemoryBlock mb_copy(mb);
    MemoryBlock mb_move(std::move(mb));
    assert('a' == mb_move[0]);
    assert('a' == mb_move[UNIT_MB]);
    }

    
    {
    //compare
    MemoryBlock mb(8);
    mb.Fill(0);
    MemoryBlock mb_assig = mb;
    MemoryBlock mb_copy(mb);

    assert(mb == mb_assig);
    assert(mb == mb_copy);
    assert(mb_assig == mb_copy);

    MemoryBlock mb_large(16);
    MemoryBlock mb_small(8);

    assert(mb_large > mb_small);
    assert(mb_small < mb_large);

    mb.Fill('a');
    mb_copy.Fill('b');

    assert(mb != mb_copy);
    assert(mb < mb_copy);

    }

    {
    MemoryBlock mb(8);
    mb.Fill('a');
    for(size_t i=0; i<mb.len(); i++)
    {
        MY_ASSERT('a' == mb[i]);
    }

    for(size_t i=0; i<mb.len(); i++)
    {
        mb[i] = 'b';
    }

    for(size_t i=0; i<mb.len(); i++)
    {
        MY_ASSERT('b' == mb[i]);
    }


    }

    return true;
}
//---------------------------------------------------------------------------
