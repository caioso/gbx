#pragma once

#include <memory>
#include <vector>

#include "ConstructionSyntacticAnalyzer.h"
#include "ExpressionIntermediateRepresentation.h"

namespace gbxasm::frontend::parsers
{

enum class ExpressionParserTreeSymbols
{
    TerminalIdentifier,
    TerminalBinaryOperatorPlus,
    NonTerminalBinaryOperator,
    NonTerminalExpression,
    NonTerminalCompoundExpression,
};

typedef struct ExpressionCompoundSymbol_t
{
    std::string Lexeme;
    ExpressionParserTreeSymbols Symbol;
}
ExpressionCompoundSymbol;

class ExpressionSyntacticAnalyzer : public interfaces::ConstructionSyntacticAnalyzer
{
public:
    ExpressionSyntacticAnalyzer() = default;
    virtual ~ExpressionSyntacticAnalyzer() = default;

    ExpressionSyntacticAnalyzer(const ExpressionSyntacticAnalyzer&) = default;
    ExpressionSyntacticAnalyzer(ExpressionSyntacticAnalyzer&&) = default;
    ExpressionSyntacticAnalyzer& operator=(const ExpressionSyntacticAnalyzer&) = default;
    ExpressionSyntacticAnalyzer& operator=(ExpressionSyntacticAnalyzer&&) = default;

    std::shared_ptr<gbxasm::intermediate_representation::IntermediateRepresentation> TryToAccept(std::vector<Token>::iterator&, std::vector<Token>::iterator&) override;

private:
    void ExtractSymbols(std::vector<Token>::iterator&, std::vector<Token>::iterator&);

    std::vector<ExpressionCompoundSymbol> _symbols;
};

}