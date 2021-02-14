#include "ConstructionPack.h"

using namespace gbxasm::interfaces;
using namespace std;

namespace gbxasm::constructions
{

ConstructionPack::ConstructionPack(size_t line, size_t column)
    :  Construction(line, column)
{
    // Test
    DeclaredMember member1 = 
    {
        .Type = TypeByte,
        .ArrayLength = 0,
        .IsArray = false,
        .Identifier = "MY_BYTE_MEMBER"
    };
    
    DeclaredMember member2 = 
    {
        .Type = TypeWord,
        .ArrayLength = 0,
        .IsArray = false,
        .Identifier = "MY_WORD_MEMBER"
    };
    
    DeclaredMember member3 = 
    {
        .Type = TypeBool,
        .ArrayLength = 0,
        .IsArray = false,
        .Identifier = "MY_BOOL_MEMBER"
    };
    
    DeclaredMember member4 = 
    {
        .Type = TypeChar,
        .ArrayLength = 0,
        .IsArray = false,
        .Identifier = "MY_CHAR_MEMBER"
    };
    
    DeclaredMember member5 = 
    {
        .Type = TypeDoubleWord,
        .ArrayLength = 0,
        .IsArray = false,
        .Identifier = "MY_DWRD_MEMBER"
    };
    
    DeclaredMember member6 = 
    {
        .Type = TypeString,
        .ArrayLength = 20,
        .IsArray = true,
        .Identifier = "MY_STRING_MEMBER"
    };

    _members.push_back(member1);
    _members.push_back(member2);
    _members.push_back(member3);
    _members.push_back(member4);
    _members.push_back(member5);
    _members.push_back(member6);

}

string ConstructionPack::Identifier()
{
    return string("MY_PACK");
}

vector<DeclaredMember> ConstructionPack::Members()
{
    return _members;
}

}