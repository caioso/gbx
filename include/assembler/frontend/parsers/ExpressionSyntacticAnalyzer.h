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
    TerminalPlus,
    TerminalMinus,
    TerminalMultiplication,
    TerminalDivision,
    TerminalBitwiseAnd,
    TerminalBitwiseOr,
    TerminalBitwiseXor,
    TerminalLeftShift,
    TerminalRightShift,
    TerminalEquality,
    TerminalDifference,
    TerminalLogicAnd,
    TerminalLogicOr,
    TerminalGreaterThan,
    TerminalLessThan,
    TerminalGreaterThanOrEqualTo,
    TerminalLessThanOrEqualTo,
    TerminalThreeWay,
    TerminalAssignment,

    TerminalIdentifier,
    
    TerminalOpenParenthesis,
    TerminalCloseParenthesis,
    
    NonTerminalExpression,                 // E
    NonTerminalSurroundedOperation,        // E5
    NonTerminalOperand,                    // E6
    NonTerminalOperation,                  // E7
    NonTerminalBinaryAddition,             // F1
    NonTerminalBinarySubtraction,          // F2
    NonTerminalBinaryMultiplication,       // F3
    NonTerminalBinaryDivision,             // F4
    NonTerminalBinaryBitwiseAnd,           // F5
    NonTerminalBinaryBitwiseOr,            // F6
    NonTerminalBinaryBitwiseXor,           // F7
    NonTerminalBinaryLeftShift,            // F8
    NonTerminalBinaryRightShift,           // F9
    NonTerminalBinaryEquality,             // F10
    NonTerminalBinaryDiffertence,          // F11
    NonTerminalBinaryLogicAnd,             // F12
    NonTerminalBinaryLogicOr,              // F13
    NonTerminalBinaryGreaterThan,          // F14
    NonTerminalBinaryLessThan,             // F15
    NonTerminalBinaryGreaterThanOrEqualTo, // F16
    NonTerminalBinaryLessThanOrEqualTo,    // F17
    NonTerminalBinaryThreeWay,             // F18
    NonTerminalBinaryAssignment,           // F19
    NonTerminalIdentifier,                 // T4
};

enum class NextOperation
{
    Reduced,
    Rejected,
    Accepted,
    Shift
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
    void ExtractSymbols(std::vector<Token>::iterator&, std::vector<Token>::iterator&);

    NextOperation EvaluateOperand(int&, size_t&);

    std::vector<ExpressionCompoundSymbol> _symbols;
    std::stack<intermediate_representation::ExpressionMember> _expressionStack;

    size_t _line;
    size_t _column;
};

}