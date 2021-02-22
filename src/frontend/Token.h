#pragma once

#include <string>

namespace gbxasm::frontend
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
    KeywordVAR,
    KeywordBOOL,
    KeywordCHAR,
    KeywordBYTE,
    KeywordWORD,
    KeywordDWRD,
    KeywordSTR,
    KeywordAS,
    KeywordCNST,
    KeywordFREE,
    KeywordIF,
    KeywordTHEN,
    KeywordELSE,
    KeywordWITH,
    KeywordREPT,
    KeywordNEXT,
    KeywordEXIT,
    KeywordWHEN,
    KeywordIS,
    KeywordFOR,
    KeywordUNTL,
    KeywordTAG,
    KeywordTRY,
    KeywordEXPT,
    KeywordTHRW,
    KeywordTEST,
    KeywordMOVE,
    KeywordHIGH,
    KeywordLOW,
    KeywordIN,
    KeywordOUT,

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
    InstructionMnemonicOR,
    InstructionMnemonicCP,
    InstructionMnemonicRET,
    InstructionMnemonicPOP,
    InstructionMnemonicJP,
    InstructionMnemonicPUSH,
    InstructionMnemonicCALL,
    InstructionMnemonicRST,
    InstructionMnemonicEI,
    InstructionMnemonicDI,
    InstructionMnemonicRLC,
    InstructionMnemonicRRC,
    InstructionMnemonicRL,
    InstructionMnemonicRR,
    InstructionMnemonicSLA,
    InstructionMnemonicSRA,
    InstructionMnemonicSWAP,
    InstructionMnemonicSRL,
    InstructionMnemonicBIT,
    InstructionMnemonicRES,
    InstructionMnemonicSET,

    // Identifier
    Identifier
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