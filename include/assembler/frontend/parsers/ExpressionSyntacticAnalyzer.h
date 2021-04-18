#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "ConstructionSyntacticAnalyzer.h"
#include "ExpressionIntermediateRepresentation.h"
#include "ExpressionMember.h"
#include "GBXAsmExceptions.h"

namespace gbxasm::frontend::parsers
{

enum class ExpressionParserTreeSymbols
{
    TerminalIgnore,
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
    TerminalLogicNegation,
    TerminalBitwiseNegation,
    TerminalOperatorDot,

    TerminalIdentifier,
    TerminalPackIdentifier,
    TerminalNumericLiteral,
    TerminalStringLiteral,
    TerminalCharLiteral,
    TerminalBooleanLiteral,
    
    TerminalOpenParenthesis,
    TerminalCloseParenthesis,
    
    NonTerminalExpression,                 // E
    NonTerminalSurroundedOperation,        // E5
    NonTerminalOperand,                    // E6
    NonTerminalUnaryOperation,             // E0
    NonTerminalBinaryOperation,            // E7
    NonTerminalUnaryLogicNegation,         // E1
    NonTerminalUnaryBitwiseNegation,       // E2
    NonTerminalUnaryNegative,              // E3
    NonTerminalUnaryPositive,              // E4
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
    NonTerminalNumericLiteral,             // T1
    NonTerminalCharLiteral,                // T2
    NonTerminalStringLiteral,              // T3
    NonTerminalIdentifier,                 // T4
    NonTerminalPackIdentifier,             // T5
    NonTerminalBooleanLiteral,             // T6
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
    intermediate_representation::OperandType SymbolToOperandType(ExpressionParserTreeSymbols);
    
    void PushResolveOperand(size_t, ExpressionCompoundSymbol, intermediate_representation::ExpressionMember&);
    void RegisterBinaryOperation(ExpressionParserTreeSymbols, intermediate_representation::ExpressionMember&);
    void RegisterUnaryOperation(ExpressionParserTreeSymbols, intermediate_representation::ExpressionMember&);
    
    NextOperation EvaluateOperand(int&, size_t&, intermediate_representation::ExpressionMember&);

    std::vector<ExpressionCompoundSymbol> _symbols;
    std::stack<intermediate_representation::ExpressionMember> _expressionStack;

    size_t _line;
    size_t _column;
    size_t _expressionID;
};

}