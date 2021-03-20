#pragma once

#include <iostream>
#include <limits>
#include <memory>
#include <vector>

#include "ConstructionSyntacticAnalyzer.h"
#include "DeclaredMember.h"
#include "FUNCIntermediateRepresentation.h"
#include "Lexemes.h"
#include "LexemeToDeclaredMemberType.h"
#include "Token.h"

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
    TerminalNumericLiteral,
    TerminalCloseBracket,
    TerminalEnd,
    TerminalIgnore,
    NonTerminalHeader,
    NonTerminalArgument,
    NonTerminalArgumentList,
    NonTerminalBody,
    NonTerminalFunc,
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
    size_t _line;
    size_t _column;
    std::vector<Token> _bodyTokens;
};

}