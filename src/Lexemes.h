#pragma once

#include <string>

namespace gbxasm
{

class Lexemes
{
public:
    // Keywords
    inline const static std::string KeywordPACK = "PACK";

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