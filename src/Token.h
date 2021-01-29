#pragma once

#include <string>

namespace gbxasm
{

enum class TokenType
{
    // Numeric Literals
    LiteralNumericHEXADECIMAL,
    LiteralNumericDECIMAL,
    LiteralNumericOCTAL,
    LiteralNumericBINARY,

    // Boolean Literals
    LiteralBooleanTRUE,
    LiteralBooleanFALSE,

    // String Literals
    LiteralSTRING,

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
    KeywordWITH,
    KeywordREPT,
    KeywordTIMES,
    KeywordNEXT,
    KeywordEXIT,
    KeywordWHEN,
    KeywordIS,
    KeywordWHILE,
    KeywordALIAS,
    KeywordTRY,
    KeywordCATCH,
    KeywordABORT,
    KeywordTEST,
    KeywordMACRO,
    KeywordMOVE,
    KeywordHIGH,
    KeywordLOW,
    KeywordBIT,
    KeywordCHECK,
    KeywordASSRT,
    KeywordPASS,
    KeywordFAIL,

    // Operators
    OperatorASSIGNMENT,
    OperatorEQUAL,
    OperatorPLUS,
    OperatorTHREEWAYCOMPARISON,
    OperatorMINUS,
    OperatorMULTIPLICATION,
    OperatorBITWISEAND,
    OperatorBITWISEOR,
    OperatorBITWISENOT,
    OperatorBITWISEXOR,
    OperatorLEFTSHIFT,
    OperatorRIGHTSHIFT,
    OperatorDIFFERENT,
    OperatorLOGICAND,
    OperatorLOGICOR,
    OperatorLESSTHAN,
    OperatorGREATERTHAN,
    OperatorLESSTHANOREQUALTO,
    OperatorGREATERTHANOREQUALTO,
    OperatorLOGICNOT,
    OperatorAT,
    OperatorSEMICOLON,
    OperatorDOT,

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