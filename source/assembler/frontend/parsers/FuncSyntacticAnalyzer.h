#pragma once

#include <iostream>
#include <memory>
#include <vector>

#include "../../interfaces/ConstructionSyntacticAnalyzer.h"
#include "../Token.h"

namespace gbxasm::frontend::parsers
{

enum class FuncParseTreeSymbols
{
    TerminalFunc,
    TerminalIdentifier,
    TerminalBgn,
    TerminalIn,
    TerminalSemicolon,
    TerminalOut,
    TerminalAs,
    TerminalType,
    TerminalOpenBracket,
    TerminalOpenNumericLiteral,
    TerminalCloseBracket,
    TerminalEnd,
    TerminalIgnore,
    NonTerminalHeader,
    NonTerminalArgument,
    NonTerminalArgumentList,
    NonTerminalBody,
    NonTerminalFooter,
};

typedef struct FuncCompoundSymbol_t
{
    std::string Lexeme;
    FuncParseTreeSymbols Symbol;
}
FuncCompoundSymbol;

class FuncSyntacticAnalyzer : public interfaces::ConstructionSyntacticAnalyzer
{
public:
    FuncSyntacticAnalyzer() = default;
    virtual ~FuncSyntacticAnalyzer() = default;

    FuncSyntacticAnalyzer(const FuncSyntacticAnalyzer&) = default;
    FuncSyntacticAnalyzer(FuncSyntacticAnalyzer&&) = default;
    FuncSyntacticAnalyzer& operator=(const FuncSyntacticAnalyzer&) = default;
    FuncSyntacticAnalyzer& operator=(FuncSyntacticAnalyzer&&) = default;

    std::shared_ptr<gbxasm::intermediate_representation::IntermediateRepresentation> TryToAccept(std::vector<Token>::iterator&, std::vector<Token>::iterator&) override;

private:
    inline void Shift(int&);

    void ExtractSymbols(std::vector<Token>::iterator&, std::vector<Token>::iterator&);
    size_t CountEndWithinFunctionBody(std::vector<Token>::iterator&, std::vector<Token>::iterator&);
    std::vector<FuncCompoundSymbol> _symbols;
};

}