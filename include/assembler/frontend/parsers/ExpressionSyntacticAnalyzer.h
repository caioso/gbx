#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "ConstructionSyntacticAnalyzer.h"
#include "ExpressionIntermediateRepresentation.h"

namespace gbxasm::frontend::parsers
{

enum class ExpressionParserTreeSymbols
{
    TerminalIdentifier,
    TerminalBooleanLiteral,
    TerminalNumericLiteral,
    TerminalStringLiteral,
    TerminalCharLiteral,
    TerminalOperatorDot,
    TerminalBinaryOperatorPlus,
    TerminalBinaryOperatorMinus,
    TerminalBinaryOperatorMultiplication,
    TerminalBinaryOperatorDivision,
    TerminalBinaryOperatorRightShift,
    TerminalBinaryOperatorLeftShift,
    TerminalBinaryOperatorLessThan,
    TerminalBinaryOperatorLessThanEqual,
    TerminalBinaryOperatorGreaterThan,
    TerminalBinaryOperatorGreaterThanEqual,
    TerminalBinaryOperatorEqual,
    TerminalBinaryOperatorDifferent,
    TerminalBinaryOperatorBitwiseAnd,
    TerminalBinaryOperatorBitwiseXor,
    TerminalBinaryOperatorBitwiseOr,
    TerminalBinaryOperatorLogicAnd,
    TerminalBinaryOperatorLogicOr,
    TerminalBinaryOperatorAssignment,
    TerminalIgnore,
    NonTerminalBinaryExpression,
    NonTerminalCompoundExpression,
    NonTerminalExpression,
};

typedef struct ExpressionCompoundSymbol_t
{
    std::string Lexeme;
    size_t Line;
    size_t Column;
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
    void PushIdentifier(ExpressionCompoundSymbol);
    void PushPackIdentifier(ExpressionCompoundSymbol, ExpressionCompoundSymbol);
    void PushBinaryOperator(ExpressionCompoundSymbol);
    
    void ReduceBinaryExpression(int, bool);
    void ReduceExpression(int);

    void ExtractSymbols(std::vector<Token>::iterator&, std::vector<Token>::iterator&);

    std::vector<ExpressionCompoundSymbol> _symbols;
    std::stack<intermediate_representation::ExpressionMember> _expressionStack;

    size_t _line;
    size_t _column;
};

}