#pragma once

#include <algorithm>
#include <iostream>
#include <vector>
#include <sstream>
#include <stack>

#include "../../GBXAsmExceptions.h"
#include "../../interfaces/ConstructionSyntacticAnalyzer.h"
#include "../../intermediate-representation/IntermediateRepresentation.h"
#include "../../intermediate-representation/PackIntermediateRepresentation.h"
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
    NonTerminalPack,
};

class PackSyntacticAnalyzer : public interfaces::ConstructionSyntacticAnalyzer
{
public:
    PackSyntacticAnalyzer() = default;
    virtual ~PackSyntacticAnalyzer() = default;

    virtual std::shared_ptr<gbxasm::intermediate_representation::IntermediateRepresentation> TryToAccept(std::vector<Token>::iterator&, std::vector<Token>::iterator&) override;

private:
    void ExtactSymbols(std::vector<Token>::iterator&, std::vector<Token>::iterator&);
    std::shared_ptr<gbxasm::intermediate_representation::IntermediateRepresentation> ExtractConstructions();

    std::vector<PackParseTreeSymbols> _symbols;
    std::stack<PackParseTreeSymbols> _stack;
};

}