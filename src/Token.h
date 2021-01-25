#pragma once

#include <string>

namespace gbxasm
{

enum class TokenType
{
    // Keywords
    KeywordPACK,
    KeywordFUNC,
    KeywordEND,
    KeywordDECL,
    KeywordBOOL,
    KeywordCHAR,
    KeywordBYTE,
    KeywordWORD,
    KeywordDWORD,
    KeywordSTR,
    KeywordAS,
    KeywordCONST,
    KeywordFREE,
    KeywordIF,
    KeywordTHEN,
    KeywordELSE,
    KeywordCALL,
    KeywordRET,
    KeywordREPT,

    // Operators
    OperatorASSIGNMENT,
    OperatorEQUAL,
    OperatorPLUS,
    OperatorTHREEWAYCOMPARISON,

    // Separators
    SeparatorCOMMA,
    SeparatorOPENPARENTHESIS,
    SeparatorCLOSEPARENTHESIS,
    SeparatorOPENCURLYBRACKETS,
    SeparatorCLOSECURLYBRACKETS,
    SeparatorOPENBRACKETS,
    SeparatorCLOSEBRACKETS,

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