#pragma once

#include <vector>

#include "Token.h"
#include "Symbol.h"

namespace gbxasm
{

class InstructionParser
{
public:
    InstructionParser(std::vector<Token>&);
    void ParseTokens();

protected:
    SymbolType DetectSymbolType(Token);

    std::vector<Token>& _tokens;
    std::vector<Symbol> _symbolTable;
};

}