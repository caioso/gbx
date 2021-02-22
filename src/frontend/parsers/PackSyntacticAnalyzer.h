#pragma once

#include <algorithm>
#include <vector>
#include <sstream>
#include <stack>

#include "../../GBXAsmExceptions.h"
#include "../../interfaces/ConstructionSyntacticAnalyzer.h"
namespace gbxasm::frontend::parsers
{

enum class PackParseTreeSymbols
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

class PackSyntacticAnalyzer : public interfaces::ConstructionSyntacticAnalyzer
{
public:
    PackSyntacticAnalyzer() = default;
    virtual ~PackSyntacticAnalyzer() = default;

    virtual AcceptedConstruction TryToAccept(std::vector<Token>::iterator&, std::vector<Token>::iterator&) override;

private:
    void ExtactSymbols(std::vector<Token>::iterator&, std::vector<Token>::iterator&);

    std::vector<PackParseTreeSymbols> _symbols;
    std::stack<PackParseTreeSymbols> _stack;
};

}