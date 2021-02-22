#pragma once

#include <cstdint>
#include <string>

namespace gbxasm::language
{

enum class TypeName
{
    Byte,
    Word,
    DoubleWord,
    Bool,
    Char,
    String
};

struct DataType
{
    TypeName Name;
    size_t Size;
};

struct DeclaredMember
{
    DataType Type;
    size_t ArrayLength;
    bool IsArray;
    std::string Identifier;
};

inline static DataType TypeByte = {.Name = TypeName::Byte, .Size = 1llu};
inline static DataType TypeWord = {.Name = TypeName::Word, .Size = 2llu};
inline static DataType TypeDoubleWord = {.Name = TypeName::DoubleWord, .Size = 4llu};
inline static DataType TypeBool = {.Name = TypeName::Bool, .Size = 1llu};
inline static DataType TypeChar = {.Name = TypeName::Char, .Size = 1llu};
inline static DataType TypeString = {.Name = TypeName::String, .Size = 1llu};

}