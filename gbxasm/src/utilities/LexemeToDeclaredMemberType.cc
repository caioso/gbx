#include "LexemeToDeclaredMemberType.h"

using namespace gbxasm;
using namespace gbxasm::frontend;
using namespace gbxasm::intermediate_representation;
using namespace std;

namespace gbxasm::utilities
{

TypeName LexemeToDeclaredMemberType::Convert(std::string lexeme)
{
    if (Lexemes::KeywordBYTE.compare(lexeme) == 0)
        return TypeName::Byte;
    else if (Lexemes::KeywordWORD.compare(lexeme) == 0)
        return TypeName::Word;
    else if (Lexemes::KeywordDWRD.compare(lexeme) == 0)
        return TypeName::DoubleWord;
    else if (Lexemes::KeywordBOOL.compare(lexeme) == 0)
        return TypeName::Bool;
    else if (Lexemes::KeywordCHAR.compare(lexeme) == 0)
        return TypeName::Char;
    else if (Lexemes::KeywordSTR.compare(lexeme) == 0)
        return TypeName::String;
    
    stringstream ss;
    ss << "Invalid type lexeme '" << lexeme << '\'';
    throw ConversionException(ss.str());
}

}