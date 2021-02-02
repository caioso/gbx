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
    
    // Char Literals
    LiteralCHAR,

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
    SeparatorDOUBLEQUOTES,

    // Instructions Mnemonics
    InstructionMnemonicNOP,
    InstructionMnemonicLD,
    InstructionMnemonicINC,
    InstructionMnemonicDEC,
    InstructionMnemonicRLCA,
    InstructionMnemonicADD,
    InstructionMnemonicRRCA,
    InstructionMnemonicSTOP,
    InstructionMnemonicRLA,
    InstructionMnemonicJR,
    InstructionMnemonicRRA,
    InstructionMnemonicDAA,
    InstructionMnemonicCPL,
    InstructionMnemonicSCF,
    InstructionMnemonicCCF,
    InstructionMnemonicHALT,
    InstructionMnemonicADC,
    InstructionMnemonicSUB,
    InstructionMnemonicSBC,
    InstructionMnemonicAND,
    InstructionMnemonicXOR,

    // Unknown
    UnknownToken
};

struct Token
{
    size_t Line;
    size_t Column;
    size_t GlobalPosition;
    std::string Lexeme;
    TokenType Type;
};

}