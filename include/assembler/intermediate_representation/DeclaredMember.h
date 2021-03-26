#pragma once

#include <cstdint>
#include <string>

namespace gbxasm::intermediate_representation
{

enum class TypeName
{
    Byte,
    Word,
    DoubleWord,
    Bool,
    Char,
    String,
    Custom,
    Unknown
};

struct DataType
{
    TypeName Name;
    std::string LexicalTypeName;
    size_t Size;
};

struct DeclaredMember
{
    DataType Type;
    std::string ArrayLength;
    bool IsArray;
    std::string Identifier;
};

enum class ArgumentDirection
{
    Input,
    Output
};

struct DeclaredArgument : DeclaredMember
{
    ArgumentDirection Direction;
};

inline static DataType TypeByte = {.Name = TypeName::Byte, .LexicalTypeName = "Byte", .Size = 1llu};
inline static DataType TypeWord = {.Name = TypeName::Word, .LexicalTypeName = "Word", .Size = 2llu};
inline static DataType TypeDoubleWord = {.Name = TypeName::DoubleWord, .LexicalTypeName = "DoubleWord", .Size = 4llu};
inline static DataType TypeBool = {.Name = TypeName::Bool, .LexicalTypeName = "Bool", .Size = 1llu};
inline static DataType TypeChar = {.Name = TypeName::Char, .LexicalTypeName = "Char", .Size = 1llu};
inline static DataType TypeString = {.Name = TypeName::String, .LexicalTypeName = "String", .Size = 1llu};
inline static DataType TypeUnknown = {.Name = TypeName::Unknown, .LexicalTypeName = "Unknown", .Size = 1llu};

DataType GetTypeByName(TypeName);

}