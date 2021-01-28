#pragma once

#include <algorithm>
#include <vector>
#include <sstream>
#include <string_view>
#include <utility>

#include <iostream>

#include "GBXAsmExceptions.h"
#include "Lexemes.h"
#include "Token.h"

namespace gbxasm
{

class Lexer
{
public:
    Lexer();
    ~Lexer() = default;

    void Tokenize(std::string_view);
    std::vector<Token>& Tokens();

private:
    std::vector<Token> EvaluateLexeme(std::string, size_t);
    std::vector<std::pair<std::string, size_t> > FindSubLexemes(std::string, size_t);
    inline std::string ExtractOperatorOrSeparator(std::string, size_t);
    inline std::string ExtractOperator(std::string, size_t);
    inline std::string ExtractSeparator(std::string, size_t);
    
    void ClearTokens();
    void ExtractTokens(std::string_view);

    inline bool IsPossibleOperator(std::string_view, size_t);
    inline bool IsPossibleSeparator(std::string_view, size_t);
    inline bool IsSeparatorOrOperator(std::string_view, size_t);
    inline bool IsNumericLiteral(std::string_view);
    inline bool IsDigit(std::string_view, size_t);
    inline bool IsInitialDigit(std::string_view, size_t);

    inline TokenType IdentifyNumericLiteral(std::string_view);
    
    inline void SaveSubLexeme(std::string, size_t, std::vector<std::pair<std::string, size_t> >&, size_t&);
    inline void CorrectLoopIndex(std::vector<std::pair<std::string, size_t> >&, size_t&); 

    inline void ValidateDecimalLiteral(std::string_view);
    inline void ValidateOctalLiteral(std::string_view);
    inline void ValidateBinaryLiteral(std::string_view);

    std::vector<Token> _tokens;
};

}