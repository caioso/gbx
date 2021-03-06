#include "DeclaredMember.h"

namespace gbxasm::intermediate_representation
{

DataType GetTypeByName(TypeName name)
{
    switch (name)
    {
        case TypeName::Byte: return TypeByte;
        case TypeName::Word: return TypeWord;
        case TypeName::DoubleWord: return TypeDoubleWord;
        case TypeName::Bool: return TypeBool;
        case TypeName::Char: return TypeChar;
        case TypeName::String: return TypeString;
    }
}

}