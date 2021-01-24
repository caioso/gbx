#pragma once

#include <string>

namespace gbxasm
{

class Lexemes
{
public:
    // Keywords
    inline const static std::string LexemeKeywordPACK = "PACK";

    // Operators
    inline const static std::string LexemeOperatorASSIGNMENT = "=";
    inline const static std::string LexemeOperatorEQUAL = "==";
    inline const static std::string LexemeOperatorPLUS = "+";
    inline const static std::string LexemeOperatorTRHEEWAYCOMPARISON = "<=>";
};

}