#pragma once

#include <string>

namespace gbxasm
{

class Lexemes
{
public:
    // Keywords
    inline const static std::string KeywordPACK = "PACK";
    inline const static std::string KeywordFUNC = "FUNC";
    inline const static std::string KeywordEND = "END";
    inline const static std::string KeywordDECL = "DECL";
    inline const static std::string KeywordBOOL = "BOOL";
    inline const static std::string KeywordCHAR = "CHAR";
    inline const static std::string KeywordBYTE = "BYTE";
    inline const static std::string KeywordWORD = "WORD";
    inline const static std::string KeywordDWORD = "DWORD";
    inline const static std::string KeywordSTR = "STR";
    inline const static std::string KeywordAS = "AS";
    inline const static std::string KeywordCONST = "CONST";
    inline const static std::string KeywordFREE = "FREE";
    inline const static std::string KeywordIF = "IF";
    inline const static std::string KeywordTHEN = "THEN";
    inline const static std::string KeywordELSE = "ELSE";
    inline const static std::string KeywordWITH = "WITH";
    inline const static std::string KeywordREPT = "REPT";
    inline const static std::string KeywordTIMES = "TIMES";
    inline const static std::string KeywordNEXT = "NEXT";
    inline const static std::string KeywordEXIT = "EXIT";
    inline const static std::string KeywordWHEN = "WHEN";
    inline const static std::string KeywordIS = "IS";
    inline const static std::string KeywordWHILE = "WHILE";
    inline const static std::string KeywordALIAS = "ALIAS";
    inline const static std::string KeywordTRY = "TRY";
    inline const static std::string KeywordCATCH = "CATCH";
    inline const static std::string KeywordABORT = "ABORT";
    inline const static std::string KeywordTEST = "TEST";
    inline const static std::string KeywordMACRO = "MACRO";
    inline const static std::string KeywordMOVE = "MOVE";
    inline const static std::string KeywordHIGH = "HIGH";
    inline const static std::string KeywordLOW = "LOW";
    inline const static std::string KeywordBIT = "BIT";

    // Operators
    inline const static std::string OperatorASSIGNMENT = "=";
    inline const static std::string OperatorEQUAL = "==";
    inline const static std::string OperatorPLUS = "+";
    inline const static std::string OperatorTRHEEWAYCOMPARISON = "<=>";
    inline const static std::string OperatorMINUS = "-";
    inline const static std::string OperatorMULTIPLICATION = "*";
    inline const static std::string OperatorBITWISEAND = "&";
    inline const static std::string OperatorBITWISEOR = "|";
    inline const static std::string OperatorBITWISENOT = "~";
    inline const static std::string OperatorBITWISEXOR = "^";
    inline const static std::string OperatorLEFTSHIFT = "<<";
    inline const static std::string OperatorRIGHTSHIFT = ">>";
    inline const static std::string OperatorDIFFERENT = "!=";
    inline const static std::string OperatorLOGICAND = "&&";
    inline const static std::string OperatorLOGICOR = "||";
    inline const static std::string OperatorLESSTHAN = "<";
    inline const static std::string OperatorGREATERTHAN = ">";
    inline const static std::string OperatorLESSTHANOREQUALTO = "<=";
    inline const static std::string OperatorGREATERTHANOREQUALTO = ">=";
    inline const static std::string OperatorLOGICNOT = "!";
    inline const static std::string OperatorAT = "@";
    inline const static std::string OperatorSEMICOLON = ":";
    inline const static std::string OperatorDOT = ".";

    // Separators
    inline const static std::string SeparatorCOMMA = ",";
    inline const static std::string SeparatorOPENPARENTHESIS = "(";
    inline const static std::string SeparatorCLOSEPARENTHESIS = ")";
    inline const static std::string SeparatorOPENCURLYBRACKETS = "{";
    inline const static std::string SeparatorCLOSECURLYBRACKETS = "}";
    inline const static std::string SeparatorOPENBRACKETS = "[";
    inline const static std::string SeparatorCLOSEBRACKETS = "]";

    // Literals
    inline const static std::string LiteralNumericHEXADECIMALBASE = "0x";
    inline const static std::string LiteralNumericDECIMALBASE = "0d";
    inline const static std::string LiteralNumericOCTALBASE = "0o";
    inline const static std::string LiteralNumericBINARYBASE = "0b";
};

}