#pragma once

#include <string>
#include <sstream>
#include <vector>

#include "Token.h"

namespace gbxasm
{

class Tokenizer
{
public:
    Tokenizer() = default;
    ~Tokenizer() = default;

    void ToToken(std::string);
    std::vector<Token>& Tokens();

protected:
    void ExtactTokens();

    std::string _input;
    std::vector<Token> _tokens;
};

}