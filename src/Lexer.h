#pragma once

#include <vector>
#include <sstream>
#include <string_view>

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
    Token EvaluateLexeme(std::string);
    std::vector<std::string> TrySubdivideLexeme(std::string);
    void ClearTokens();
    void ExtractTokens(std::string_view);

    std::vector<Token> _tokens;
};

}