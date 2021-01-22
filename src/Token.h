#pragma once

#include <string>

namespace gbxasm
{

enum class TokenType
{
    // Keywords
    KeywordPACK,


    // Operators
    OperatorPLUS,

    // Unknown
    UnknownToken
};

struct Token
{
    size_t Line;
    size_t Column;
    std::string Lexeme;
    TokenType Type;
};

}