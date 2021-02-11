#pragma once

#include <algorithm>
#include <vector>
#include <sstream>
#include <stack>

#include "../GBXAsmExceptions.h"
#include "../interfaces/ConstructionParser.h"

#include <iostream>

namespace gbxasm::parsers
{

enum class PackSymbols
{
    TerminalPack,
    TerminalType,
    TerminalIdentifier,
    TerminalOpenBracket,
    TerminalNumericLiteral,
    TerminalCloseBracket,
    TerminalEnd,
    NonTerminalHeader,
    NonTerminalMember,
    NonTerminalMemberList,
    NonTerminalFooter,
};

class PackParser : public interfaces::ConstructionParser
{
public:
    PackParser() = default;
    virtual ~PackParser() = default;

    virtual AcceptedConstruction TryToAccept(std::vector<Token>::iterator&, std::vector<Token>::iterator&) override;

private:
    void ExtactSymbols(std::vector<Token>::iterator&, std::vector<Token>::iterator&);

    std::vector<PackSymbols> _symbols;
    std::stack<PackSymbols> _stack;
};

}