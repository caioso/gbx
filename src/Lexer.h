#pragma once

#include <vector>
#include <sstream>
#include <string_view>
#include <utility>

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
    std::string ExtractOperator(std::string, size_t);
    
    void ClearTokens();
    void ExtractTokens(std::string_view);
    
    bool IsPossibleOperator(std::string, size_t);

    std::vector<Token> _tokens;
};

}