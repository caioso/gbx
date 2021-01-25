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
    inline const static std::string KeywordCALL = "CALL";
    inline const static std::string KeywordRET = "RET";
    inline const static std::string KeywordREPT = "REPT";

    // Operators
    inline const static std::string OperatorASSIGNMENT = "=";
    inline const static std::string OperatorEQUAL = "==";
    inline const static std::string OperatorPLUS = "+";
    inline const static std::string OperatorTRHEEWAYCOMPARISON = "<=>";

    // Separators
    inline const static std::string SeparatorCOMMA = ",";
    inline const static std::string SeparatorOPENPARENTHESIS = "(";
    inline const static std::string SeparatorCLOSEPARENTHESIS = ")";
    inline const static std::string SeparatorOPENCURLYBRACKETS = "{";
    inline const static std::string SeparatorCLOSECURLYBRACKETS = "}";
    inline const static std::string SeparatorOPENBRACKETS = "[";
    inline const static std::string SeparatorCLOSEBRACKETS = "]";
};

}