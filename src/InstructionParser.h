#pragma once

#include <vector>

#include "Token.h"
#include "SymbolWrapper.h"
#include "interfaces/BaseSymbol.h"
#include "symbols/Label.h"

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
    std::vector<SymbolWrapper> _symbolTable;
};

}