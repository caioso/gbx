#include "ExpressionSyntacticAnalyzer.h"

using namespace gbxasm;
using namespace gbxasm::intermediate_representation;
using namespace std;

namespace gbxasm::frontend::parsers
{

shared_ptr<IntermediateRepresentation> ExpressionSyntacticAnalyzer::TryToAccept(vector<Token>::iterator& beginIt, vector<Token>::iterator& endIt)
{
    ExtractSymbols(beginIt, endIt);
    ExpressionMember currentMember;
    ClearCurrentMember(currentMember);

    _expressionID = -1;

    while (!IsAccepted() && !IsRejected() && !IsExited())
    {        
        auto leftSubstring = -1; 
        auto state = FSMState::InitialState;
        ExpressionParserTreeSymbols operationType;
        _depth = 0;

        while(true)
        {
            if (state == FSMState::InitialState)
            {
                leftSubstring++;
                
                if (AllExpressionsHaveBeenReduced(leftSubstring)) 
                { Accept(); break; }

                state = FSMState::EvaluateFirstOperandUnaryOperatorOrEnclosing;
            }
            else if (state == FSMState::EvaluateFirstOperandUnaryOperatorOrEnclosing)
            {
                auto resultingOperation = EvaluateOperand(leftSubstring, state, currentMember);

                if (resultingOperation == NextOperation::Rejected)
                { Reject(); break; }
                else if (resultingOperation == NextOperation::Accepted)
                { Accept(); break; }
                else if (resultingOperation == NextOperation::Reduced)
                    break;
            }
            // Further evaluate operation
            else if (state == FSMState::EvaluateBinaryOperatorOrRHSUnary)
            {
                // Move further ( Should remember that + has been parsed)
                if ((_expressionStack.top().Type == ExpressionType::ResolveOperand &&
                    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalIncrement) ||
                    (_expressionStack.top().Type == ExpressionType::ResolveOperand &&
                    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalDecrement))
                {
                    ExpressionParserTreeSymbols unaryOperationType = _symbols[leftSubstring].Symbol; 
                    RegisterUnaryOperation(unaryOperationType, currentMember, true);
                    PushUnaryExpressionOperand(leftSubstring, currentMember, true);
                    ReduceUnaryExpression(leftSubstring, unaryOperationType, true);
                    break;
                }
                else if (IsBinaryOperator(leftSubstring))
                    ProcessBinaryOperator(leftSubstring, state, operationType);
                else if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalCloseParenthesis || 
                         _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalCloseBrackets)
                {
                    // Try to reduce (E) -> E6
                    if (IsEndOfEnclosedExpression(leftSubstring))
                    {
                        ReduceEnclosedExpression(leftSubstring);
                        break;
                    }
                    else
                    { Reject(); break; }
                }
                else 
                { Reject(); break; }
            }
            // Further evaluate operation
            else if (state == FSMState::EvaluateSecondOperandUnaryOperatorOrEnclosing)
            {
                auto resultingOperation = EvaluateOperand(leftSubstring, state, currentMember);

                if (resultingOperation == NextOperation::Rejected)
                { Reject(); break; }
                else if (resultingOperation == NextOperation::Accepted)
                { Accept(); break; }
                else if (resultingOperation == NextOperation::Reduced)
                    break;
            }
            // Reduce expression of type E + E
            else if (state == FSMState::ReduceBinaryOperation)
            {
                RegisterBinaryOperation(operationType, currentMember);
                PushBinaryExpressionOperand(leftSubstring, currentMember);
                ReduceBinaryExpression(leftSubstring, operationType);
                break;
            }
            // Reduce expression of type <OP>E
            else if (state == FSMState::ReduceUnaryOperation)
            {
                if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalExpression)
                {
                    auto unaryOperationType = _symbols[leftSubstring - 1].Symbol;
                    RegisterUnaryOperation(unaryOperationType, currentMember, false);
                    PushUnaryExpressionOperand(leftSubstring, currentMember, false);
                    ReduceUnaryExpression(leftSubstring, unaryOperationType, false);
                    break;
                }
                else
                {
                    // Do not increment, simply go further
                    state = FSMState::EvaluateFirstOperandUnaryOperatorOrEnclosing;
                }
            }
        }
    }

    auto representation = make_shared<ExpressionIntermediateRepresentation>(_expressionStack, _line, _column);
    return representation;
}

void ExpressionSyntacticAnalyzer::ReduceUnaryExpression(int leftSubstring, ExpressionParserTreeSymbols unaryOperationType, bool rightHandSideUnary)
{
    // Remove Non-Terminal Expression (E) / RHS Unary Operand
    _symbols.erase(begin(_symbols) + leftSubstring);
    // Remove Unary Operand / Non-Treminal Expression
    _symbols.erase(begin(_symbols) + leftSubstring - 1);

    if (unaryOperationType == ExpressionParserTreeSymbols::TerminalLogicNegation)
        _symbols.insert(begin(_symbols) +  leftSubstring - 1, { .Symbol = ExpressionParserTreeSymbols::NonTerminalUnaryLogicNegation, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (unaryOperationType == ExpressionParserTreeSymbols::TerminalBitwiseNegation)
        _symbols.insert(begin(_symbols) +  leftSubstring - 1, { .Symbol = ExpressionParserTreeSymbols::NonTerminalUnaryBitwiseNegation, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (unaryOperationType == ExpressionParserTreeSymbols::TerminalMinus)
        _symbols.insert(begin(_symbols) +  leftSubstring - 1, { .Symbol = ExpressionParserTreeSymbols::NonTerminalUnaryNegative, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (unaryOperationType == ExpressionParserTreeSymbols::TerminalPlus)
        _symbols.insert(begin(_symbols) +  leftSubstring - 1, { .Symbol = ExpressionParserTreeSymbols::NonTerminalUnaryPositive, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (unaryOperationType == ExpressionParserTreeSymbols::TerminalOperatorHash)
        _symbols.insert(begin(_symbols) +  leftSubstring - 1, { .Symbol = ExpressionParserTreeSymbols::NonTerminalUnaryImmedite, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (unaryOperationType == ExpressionParserTreeSymbols::TerminalIncrement && !rightHandSideUnary)
        _symbols.insert(begin(_symbols) +  leftSubstring - 1, { .Symbol = ExpressionParserTreeSymbols::NonTerminalUnaryPreIncrement, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (unaryOperationType == ExpressionParserTreeSymbols::TerminalIncrement && rightHandSideUnary)
        _symbols.insert(begin(_symbols) +  leftSubstring - 1, { .Symbol = ExpressionParserTreeSymbols::NonTerminalUnaryPostIncrement, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (unaryOperationType == ExpressionParserTreeSymbols::TerminalDecrement && !rightHandSideUnary)
        _symbols.insert(begin(_symbols) +  leftSubstring - 1, { .Symbol = ExpressionParserTreeSymbols::NonTerminalUnaryPreDecrement, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (unaryOperationType == ExpressionParserTreeSymbols::TerminalDecrement && rightHandSideUnary)
        _symbols.insert(begin(_symbols) +  leftSubstring - 1, { .Symbol = ExpressionParserTreeSymbols::NonTerminalUnaryPostDecrement, .Lexeme = string(""), .Surrounder = Surrounder::None });
}

void ExpressionSyntacticAnalyzer::ReduceBinaryExpression(int leftSubstring, ExpressionParserTreeSymbols operationType)
{
    // Reduce Binary operation E <OP> E -> F1
    // Remove Identifier
    _symbols.erase(begin(_symbols) + leftSubstring - 1);
    // Remove +
    _symbols.erase(begin(_symbols) + leftSubstring - 2);
    // Remove Identifier
    _symbols.erase(begin(_symbols) + leftSubstring - 3);

    if (operationType == ExpressionParserTreeSymbols::TerminalPlus)
        _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryAddition, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (operationType == ExpressionParserTreeSymbols::TerminalMinus)
        _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinarySubtraction, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (operationType == ExpressionParserTreeSymbols::TerminalMultiplication)
        _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryMultiplication, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (operationType == ExpressionParserTreeSymbols::TerminalDivision)
        _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryDivision, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (operationType == ExpressionParserTreeSymbols::TerminalBitwiseAnd)
        _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryBitwiseAnd, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (operationType == ExpressionParserTreeSymbols::TerminalBitwiseOr)
        _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryBitwiseOr, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (operationType == ExpressionParserTreeSymbols::TerminalBitwiseXor)
        _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryBitwiseXor, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (operationType == ExpressionParserTreeSymbols::TerminalLeftShift)
        _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryLeftShift, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (operationType == ExpressionParserTreeSymbols::TerminalRightShift)
        _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryRightShift, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (operationType == ExpressionParserTreeSymbols::TerminalEquality)
        _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryEquality, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (operationType == ExpressionParserTreeSymbols::TerminalDifference)
        _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryDiffertence, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (operationType == ExpressionParserTreeSymbols::TerminalLogicAnd)
        _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryLogicAnd, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (operationType == ExpressionParserTreeSymbols::TerminalLogicOr)
        _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryLogicOr, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (operationType == ExpressionParserTreeSymbols::TerminalGreaterThan)
        _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryGreaterThan, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (operationType == ExpressionParserTreeSymbols::TerminalLessThan)
        _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryLessThan, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (operationType == ExpressionParserTreeSymbols::TerminalGreaterThanOrEqualTo)
        _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryGreaterThanOrEqualTo, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (operationType == ExpressionParserTreeSymbols::TerminalLessThanOrEqualTo)
        _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryLessThanOrEqualTo, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (operationType == ExpressionParserTreeSymbols::TerminalThreeWay)
        _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryThreeWay, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else if (operationType == ExpressionParserTreeSymbols::TerminalAssignment)
        _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryAssignment, .Lexeme = string(""), .Surrounder = Surrounder::None });
}

bool ExpressionSyntacticAnalyzer::AllExpressionsHaveBeenReduced(int leftSubstring)
{
    return _symbols.size() == 1 && _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalExpression;
}

bool ExpressionSyntacticAnalyzer::IsBinaryOperator(int leftSubstring)
{
    return _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalPlus ||
    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalMinus ||
    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalMultiplication ||
    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalDivision ||
    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalBitwiseAnd ||
    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalBitwiseOr ||
    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalBitwiseXor ||
    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalLeftShift ||
    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalRightShift ||
    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalEquality ||
    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalDifference ||
    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalLogicAnd ||
    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalLogicOr ||
    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalGreaterThan ||
    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalLessThan ||
    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalGreaterThanOrEqualTo ||
    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalLessThanOrEqualTo ||
    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalThreeWay ||
    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalAssignment;
}

void ExpressionSyntacticAnalyzer::ReduceEnclosedExpression(int leftSubstring)
{
    // Reduce Surrounded operation
    // Remove )
    auto encloserType = (*(begin(_symbols) + leftSubstring)).Symbol;
    _symbols.erase(begin(_symbols) + leftSubstring);
    // Remove E
    _symbols.erase(begin(_symbols) + leftSubstring - 1);
    // Remove (
    _symbols.erase(begin(_symbols) + leftSubstring - 2);
    
    if (encloserType == ExpressionParserTreeSymbols::TerminalCloseParenthesis)
        _symbols.insert(begin(_symbols) +  leftSubstring - 2, { .Symbol = ExpressionParserTreeSymbols::NonTerminalSurroundedOperation, .Lexeme = string(""), .Surrounder = Surrounder::None });
    else
        _symbols.insert(begin(_symbols) +  leftSubstring - 2, { .Symbol = ExpressionParserTreeSymbols::NonTerminalSurroundedMemoryOperation, .Lexeme = string(""), .Surrounder = Surrounder::None });
    
    // The expression has already been pushed, but only now it is known that it is part of a surrounded operation. Since the parethesis has been closed,
    // decrese the depth of the lastly pushed expression
    _expressionStack.top().Depth--;

    if (encloserType == ExpressionParserTreeSymbols::TerminalCloseParenthesis)
        _expressionStack.top().SurrounderType = Surrounder::Expression;
    else
        _expressionStack.top().SurrounderType = Surrounder::Memory;
}

bool ExpressionSyntacticAnalyzer::IsEndOfEnclosedExpression(int leftSubstring)
{
    return (_symbols[leftSubstring - 1].Symbol == ExpressionParserTreeSymbols::NonTerminalExpression &&
            _symbols[leftSubstring - 2].Symbol == ExpressionParserTreeSymbols::TerminalOpenParenthesis) || 
           (_symbols[leftSubstring - 1].Symbol == ExpressionParserTreeSymbols::NonTerminalExpression &&
            _symbols[leftSubstring - 2].Symbol == ExpressionParserTreeSymbols::TerminalOpenBrackets);
}

void ExpressionSyntacticAnalyzer::ProcessBinaryOperator(int& leftSubstring, FSMState& state, ExpressionParserTreeSymbols& operationType)
{
    operationType = _symbols[leftSubstring].Symbol;
    leftSubstring++;
    state = FSMState::EvaluateSecondOperandUnaryOperatorOrEnclosing;
}

NextOperation ExpressionSyntacticAnalyzer::EvaluateOperand(int& leftSubstring, FSMState& state, ExpressionMember& currentMember)
{
    if (leftSubstring >= static_cast<int>(_symbols.size()))
        return NextOperation::Rejected; 

    // NON TERMINALS
    // Detect Identifier
    if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalIdentifier)
    {
        // Reduce Identifier to Non-Terminal Identifier (T4)
        // Remove Identifier
        PushResolveOperand(_symbols[leftSubstring], currentMember);

        _symbols.erase(begin(_symbols) + leftSubstring);
        _symbols.insert(begin(_symbols) + leftSubstring, { .Symbol = ExpressionParserTreeSymbols::NonTerminalIdentifier, .Lexeme = string(""), .Surrounder = Surrounder::None });
        return NextOperation::Reduced;
    }
    // Detect Pack Identifier
    else if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalPackIdentifier)
    {
        // Reduce Identifier to Non-Terminal Identifier (T5)
        // Remove Identifier
        PushResolveOperand(_symbols[leftSubstring], currentMember);

        _symbols.erase(begin(_symbols) + leftSubstring);
        _symbols.insert(begin(_symbols) + leftSubstring, { .Symbol = ExpressionParserTreeSymbols::NonTerminalPackIdentifier, .Lexeme = string(""), .Surrounder = Surrounder::None });
        return NextOperation::Reduced;
    }
    // Detect Numeric Literal
    else if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalNumericLiteral)
    {
        // Remove Identifier
        // Reduce Identifier to Non-Terminal Identifier (T1)
        PushResolveOperand(_symbols[leftSubstring], currentMember);

        _symbols.erase(begin(_symbols) + leftSubstring);
        _symbols.insert(begin(_symbols) + leftSubstring, { .Symbol = ExpressionParserTreeSymbols::NonTerminalNumericLiteral, .Lexeme = string(""), .Surrounder = Surrounder::None });
        return NextOperation::Reduced;
    }
    // Detect Char Literal
    else if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalCharLiteral)
    {
        // Reduce Identifier to Non-Terminal Identifier (T2)
        // Remove Identifier
        PushResolveOperand(_symbols[leftSubstring], currentMember);

        _symbols.erase(begin(_symbols) + leftSubstring);
        _symbols.insert(begin(_symbols) + leftSubstring, { .Symbol = ExpressionParserTreeSymbols::NonTerminalCharLiteral, .Lexeme = string(""), .Surrounder = Surrounder::None });
        return NextOperation::Reduced;
    }
    // Detect String Literal
    else if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalStringLiteral)
    {
        // Reduce Identifier to Non-Terminal Identifier (T3)
        // Remove Identifier
        PushResolveOperand(_symbols[leftSubstring], currentMember);

        _symbols.erase(begin(_symbols) + leftSubstring);
        _symbols.insert(begin(_symbols) + leftSubstring, { .Symbol = ExpressionParserTreeSymbols::NonTerminalStringLiteral, .Lexeme = string(""), .Surrounder = Surrounder::None });
        return NextOperation::Reduced;
    }
    // Detect Boolean Literal
    else if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalBooleanLiteral)
    {
        // Reduce Identifier to Non-Terminal Identifier (T6)
        // Remove Identifier
        PushResolveOperand(_symbols[leftSubstring], currentMember);

        _symbols.erase(begin(_symbols) + leftSubstring);
        _symbols.insert(begin(_symbols) + leftSubstring, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBooleanLiteral, .Lexeme = string(""), .Surrounder = Surrounder::None });
        return NextOperation::Reduced;
    }
    // Detect ( or [
    else if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalOpenParenthesis ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalOpenBrackets)
    {
        // Do nothing, simply shift
        _depth++;
        leftSubstring++;
        state = FSMState::EvaluateFirstOperandUnaryOperatorOrEnclosing;
        return NextOperation::Shift;
    }
    // Detect Unary Operands
    else if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalLogicNegation ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalBitwiseNegation || 
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalMinus ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalPlus ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalOperatorHash ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalIncrement ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalDecrement)
    {
        // Do nothing, simply shift
        leftSubstring++;
        state = FSMState::ReduceUnaryOperation;
        return NextOperation::Shift;
    }
    // NON-TERMINALS
    else if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalIdentifier ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalNumericLiteral ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalCharLiteral || 
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalStringLiteral ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBooleanLiteral ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalPackIdentifier)
    {
        // Reduce Identifier to Non-Terminal Operand (E6)
        // Remove Non-terminal Identifier
        _symbols.erase(begin(_symbols) + leftSubstring);
        _symbols.insert(begin(_symbols) + leftSubstring, { .Symbol = ExpressionParserTreeSymbols::NonTerminalOperand, .Lexeme = string(""), .Surrounder = Surrounder::None });
        return NextOperation::Reduced;
    }
    else if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalUnaryLogicNegation ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalUnaryBitwiseNegation ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalUnaryImmedite ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalUnaryPreIncrement ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalUnaryPostIncrement ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalUnaryPreDecrement ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalUnaryPostDecrement ||
             ((state == FSMState::EvaluateFirstOperandUnaryOperatorOrEnclosing || state == FSMState::EvaluateSecondOperandUnaryOperatorOrEnclosing) && _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalUnaryNegative) ||
             ((state == FSMState::EvaluateFirstOperandUnaryOperatorOrEnclosing || state == FSMState::EvaluateSecondOperandUnaryOperatorOrEnclosing) && _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalUnaryPositive))
    {
        // Reduce Identifier to Non-Terminal Binary Operation (E1/E2/E3/E4...) to Non Terminal Unary peration (E0)
        // Remove Non-terminal Operand
        _symbols.erase(begin(_symbols) + leftSubstring);
        _symbols.insert(begin(_symbols) + leftSubstring, { .Symbol = ExpressionParserTreeSymbols::NonTerminalUnaryOperation, .Lexeme = string(""), .Surrounder = Surrounder::None });
        return NextOperation::Reduced;
    }
    else if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryAddition ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinarySubtraction ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryMultiplication ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryDivision ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryBitwiseAnd ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryBitwiseOr || 
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryBitwiseXor ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryLeftShift ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryRightShift ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryEquality ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryDiffertence ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryLogicAnd ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryLogicOr ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryGreaterThan ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryLessThan ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryGreaterThanOrEqualTo ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryLessThanOrEqualTo ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryThreeWay ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryAssignment)
    {
        // Reduce Identifier to Non-Terminal Binary Operation (F1/F2/F3/F4/F5/F6/F7/F8/F9/F10/F11/F12/F13/F14/F15/F16/F17/F18/F19) to Non Terminal Binary Operation (E7)
        // Remove Non-terminal Operand
        _symbols.erase(begin(_symbols) + leftSubstring);
        _symbols.insert(begin(_symbols) + leftSubstring, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryOperation, .Lexeme = string(""), .Surrounder = Surrounder::None });
        return NextOperation::Reduced;
    }
    else if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalOperand ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalUnaryOperation ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryOperation || 
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalSurroundedOperation ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalSurroundedMemoryOperation)
    {
        // Reduce Identifier to Non-Terminal Expression (E)
        // Remove Non-terminal Operand
        stringstream ss;
        ss << "exp" << _expressionID;
        _symbols.erase(begin(_symbols) + leftSubstring);
        _symbols.insert(begin(_symbols) + leftSubstring, { .Symbol = ExpressionParserTreeSymbols::NonTerminalExpression, .Lexeme = ss.str(), .Surrounder = Surrounder::None });

        return NextOperation::Reduced;
    }

    // NON TERMINAL EXPRESSION
    else if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalExpression)
    {
        // 'First part of a binary operation'
        if (state == FSMState::EvaluateFirstOperandUnaryOperatorOrEnclosing)
            state = FSMState::EvaluateBinaryOperatorOrRHSUnary;
        // 'Second part of a binary operation'
        else if (state == FSMState::EvaluateSecondOperandUnaryOperatorOrEnclosing)
            state = FSMState::ReduceBinaryOperation;
        
        leftSubstring++;
        return NextOperation::Shift;
    }
    else 
    {
        return NextOperation::Rejected;
    }
}

void ExpressionSyntacticAnalyzer::ExtractSymbols(std::vector<Token>::iterator& beginIt, std::vector<Token>::iterator& endIt)
{
    if (beginIt == endIt)
        return;

    // Only accept operators and identifiers for now (will be expanded with literals soon)
    transform(beginIt, endIt, back_inserter(_symbols), [&](Token x) -> ExpressionCompoundSymbol
    {
        switch (x.Type)
        {
            case TokenType::Identifier: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalIdentifier, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::LiteralNumericBINARY: 
            case TokenType::LiteralNumericOCTAL: 
            case TokenType::LiteralNumericDECIMAL: 
            case TokenType::LiteralNumericHEXADECIMAL: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalNumericLiteral, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::LiteralSTRING: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalStringLiteral, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::LiteralCHAR: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalCharLiteral, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::LiteralBooleanTRUE: 
            case TokenType::LiteralBooleanFALSE: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalBooleanLiteral, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::SeparatorOPENPARENTHESIS:
                return {.Symbol = ExpressionParserTreeSymbols::TerminalOpenParenthesis, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::SeparatorCLOSEPARENTHESIS:
                return {.Symbol = ExpressionParserTreeSymbols::TerminalCloseParenthesis, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::SeparatorOPENBRACKETS:
                return {.Symbol = ExpressionParserTreeSymbols::TerminalOpenBrackets, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::SeparatorCLOSEBRACKETS:
                return {.Symbol = ExpressionParserTreeSymbols::TerminalCloseBrackets, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorPLUS: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalPlus, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorMINUS: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalMinus, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorMULTIPLICATION: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalMultiplication, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorDIVISION: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalDivision, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorBITWISEAND: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalBitwiseAnd, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorBITWISEOR: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalBitwiseOr, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorBITWISEXOR: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalBitwiseXor, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorLEFTSHIFT: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalLeftShift, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorRIGHTSHIFT: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalRightShift, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorEQUAL: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalEquality, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorDIFFERENT: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalDifference, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorLOGICAND: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalLogicAnd, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorLOGICOR: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalLogicOr, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorGREATERTHAN: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalGreaterThan, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorLESSTHAN: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalLessThan, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorGREATERTHANOREQUALTO: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalGreaterThanOrEqualTo, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorLESSTHANOREQUALTO: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalLessThanOrEqualTo, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorTHREEWAYCOMPARISON: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalThreeWay, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorASSIGNMENT: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalAssignment, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorLOGICNOT: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalLogicNegation, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorBITWISENOT: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalBitwiseNegation, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorDOT: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalOperatorDot, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorHASH: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalOperatorHash, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorUNARYINCREMENT: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalIncrement, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorUNARYDECREMENT: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalDecrement, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            default:
                return {.Symbol = ExpressionParserTreeSymbols::TerminalIgnore, .Lexeme  = "", .Line = x.Line, .Column = x.Column};
        }
    });

    _line = (*beginIt).Line;
    _column = (*beginIt).Column;

    // TODO: Create the 'Pack Identifiers' by combining the left and right parts berfore/after the '.' found during the encoding.
    auto dotFound = true;
    while (dotFound)
    {
        dotFound = false;
        for (auto i = 0llu; i < _symbols.size(); ++i)
        {
            if (_symbols[i].Symbol == ExpressionParserTreeSymbols::TerminalOperatorDot)
            {
                dotFound = true;

                if (i == 0 || i == _symbols.size() - 1)
                    throw SyntacticAnalyzerException("Unexpected '.' found");
                else
                {
                    if (_symbols[i - 1].Symbol != ExpressionParserTreeSymbols::TerminalIdentifier)
                        throw SyntacticAnalyzerException("Expected identifier before '.'");
                    else if (_symbols[i + 1].Symbol != ExpressionParserTreeSymbols::TerminalIdentifier)
                        throw SyntacticAnalyzerException("Expected identifier after '.'");
                    
                    auto lexeme = _symbols[i - 1].Lexeme + _symbols[i].Lexeme + _symbols[i + 1].Lexeme;
                    auto column = _symbols[i].Column;
                    auto line = _symbols[i].Line;

                    _symbols.erase(begin(_symbols) + i + 1);
                    _symbols.erase(begin(_symbols) + i);
                    _symbols.erase(begin(_symbols) + i - 1);
                    _symbols.insert(begin(_symbols) + i - 1, {.Symbol = ExpressionParserTreeSymbols::TerminalPackIdentifier, .Lexeme = lexeme, .Line = line, .Column = column, .Surrounder = Surrounder::None });
                    break;
                }
            }
        }
    }
}

OperandType ExpressionSyntacticAnalyzer::SymbolToOperandType(ExpressionParserTreeSymbols symbol)
{
    if (symbol == ExpressionParserTreeSymbols::TerminalIdentifier)
        return OperandType::Identifier;
    else if (symbol == ExpressionParserTreeSymbols::TerminalPackIdentifier)
        return OperandType::PackIdentifier;
    else if (symbol == ExpressionParserTreeSymbols::TerminalNumericLiteral)
        return OperandType::NumericLiteral;
    else if (symbol == ExpressionParserTreeSymbols::TerminalCharLiteral)
        return OperandType::CharLiteral;
    else if (symbol == ExpressionParserTreeSymbols::TerminalStringLiteral)
        return OperandType::StringLiteral;
    else if (symbol == ExpressionParserTreeSymbols::TerminalBooleanLiteral)
        return OperandType::BooleanLiteral;
    else
        return OperandType::Expression;
}

void ExpressionSyntacticAnalyzer::PushResolveOperand(ExpressionCompoundSymbol symbol, ExpressionMember& currentMember)
{
    currentMember.ExpressionID = ++_expressionID;
    currentMember.Depth = _depth;
    currentMember.Type = ExpressionType::ResolveOperand;
    currentMember.OperatorType = Operator::NoOperator;
    currentMember.Operand1 = symbol.Lexeme;
    currentMember.Operand1Type = SymbolToOperandType(symbol.Symbol);   
    currentMember.Operand2 = "";
    currentMember.Operand2Type = OperandType::NoOperand;
    _expressionStack.push(currentMember);
    ClearCurrentMember(currentMember);
}

void ExpressionSyntacticAnalyzer::PushUnaryExpressionOperand(size_t leftSubstring, ExpressionMember& currentMember, bool rightHandSideUnary)
{
    currentMember.ExpressionID = ++_expressionID;
    currentMember.Depth = _depth;
    currentMember.Type = ExpressionType::Unary;
    currentMember.Operand1Type = SymbolToOperandType((*(begin(_symbols) + leftSubstring)).Symbol);   

    if (!rightHandSideUnary)
        currentMember.Operand1 = (*(begin(_symbols) + leftSubstring)).Lexeme;
    else
        currentMember.Operand1 = (*(begin(_symbols) + leftSubstring - 1)).Lexeme;

    currentMember.Operand2Type = OperandType::NoOperand;
    currentMember.Operand2 = string("");
    _expressionStack.push(currentMember);
    ClearCurrentMember(currentMember);
}

void ExpressionSyntacticAnalyzer::PushBinaryExpressionOperand(size_t leftSubstring, ExpressionMember& currentMember)
{
    currentMember.ExpressionID = ++_expressionID;
    currentMember.Depth = _depth;
    currentMember.Type = ExpressionType::Binary;
    currentMember.Operand1Type = SymbolToOperandType((*(begin(_symbols) + leftSubstring - 2)).Symbol);   
    currentMember.Operand1 = (*(begin(_symbols) + leftSubstring - 3)).Lexeme;
    currentMember.Operand2Type = SymbolToOperandType((*(begin(_symbols) + leftSubstring - 1)).Symbol);   
    currentMember.Operand2 = (*(begin(_symbols) + leftSubstring - 1)).Lexeme;
    _expressionStack.push(currentMember);
    ClearCurrentMember(currentMember);
}

void ExpressionSyntacticAnalyzer::ClearCurrentMember(ExpressionMember& currentMember)
{
    currentMember = {};
}

void ExpressionSyntacticAnalyzer::RegisterBinaryOperation(ExpressionParserTreeSymbols operationType, ExpressionMember& currentMember)
{
    currentMember.Type = ExpressionType::Binary;

    if (operationType == ExpressionParserTreeSymbols::TerminalPlus)
        currentMember.OperatorType = Operator::BinaryAddition; 
    else if (operationType == ExpressionParserTreeSymbols::TerminalMinus)
        currentMember.OperatorType = Operator::BinarySubtraction;
    else if (operationType == ExpressionParserTreeSymbols::TerminalMultiplication)
        currentMember.OperatorType = Operator::BinaryMultiplication;
    else if (operationType == ExpressionParserTreeSymbols::TerminalDivision)
        currentMember.OperatorType = Operator::BinaryDivision;
    else if (operationType == ExpressionParserTreeSymbols::TerminalBitwiseAnd)
        currentMember.OperatorType = Operator::BinaryBitwiseAnd;
    else if (operationType == ExpressionParserTreeSymbols::TerminalBitwiseOr)
        currentMember.OperatorType = Operator::BinaryBitwiseOr;
    else if (operationType == ExpressionParserTreeSymbols::TerminalBitwiseXor)
        currentMember.OperatorType = Operator::BinaryBitwiseXor;
    else if (operationType == ExpressionParserTreeSymbols::TerminalLeftShift)
        currentMember.OperatorType = Operator::BinaryLeftShift;
    else if (operationType == ExpressionParserTreeSymbols::TerminalRightShift)
        currentMember.OperatorType = Operator::BinaryRightShift;
    else if (operationType == ExpressionParserTreeSymbols::TerminalEquality)
        currentMember.OperatorType = Operator::BinaryEquality;
    else if (operationType == ExpressionParserTreeSymbols::TerminalDifference)
        currentMember.OperatorType = Operator::BinaryDiffertence;
    else if (operationType == ExpressionParserTreeSymbols::TerminalLogicAnd)
        currentMember.OperatorType = Operator::BinaryLogicAnd;
    else if (operationType == ExpressionParserTreeSymbols::TerminalLogicOr)
        currentMember.OperatorType = Operator::BinaryLogicOr;
    else if (operationType == ExpressionParserTreeSymbols::TerminalGreaterThan)
        currentMember.OperatorType = Operator::BinaryGreaterThan;
    else if (operationType == ExpressionParserTreeSymbols::TerminalLessThan)
        currentMember.OperatorType = Operator::BinaryLessThan;
    else if (operationType == ExpressionParserTreeSymbols::TerminalGreaterThanOrEqualTo)
        currentMember.OperatorType = Operator::BinaryGreaterThanOrEqualTo;
    else if (operationType == ExpressionParserTreeSymbols::TerminalLessThanOrEqualTo)
        currentMember.OperatorType = Operator::BinaryLessThanOrEqualTo;
    else if (operationType == ExpressionParserTreeSymbols::TerminalThreeWay)
        currentMember.OperatorType = Operator::BinaryThreeWay;
    else if (operationType == ExpressionParserTreeSymbols::TerminalAssignment)
        currentMember.OperatorType = Operator::BinaryAssignment;
}

void ExpressionSyntacticAnalyzer::RegisterUnaryOperation(ExpressionParserTreeSymbols operationType, ExpressionMember& currentMember, bool rightHandSideUnary)
{
    currentMember.Type = ExpressionType::Unary;

    if (operationType == ExpressionParserTreeSymbols::TerminalLogicNegation)
        currentMember.OperatorType = Operator::UnaryLogicNegation;
    else if (operationType == ExpressionParserTreeSymbols::TerminalBitwiseNegation)
        currentMember.OperatorType = Operator::UnaryBitwiseNegation;
    else if (operationType == ExpressionParserTreeSymbols::TerminalMinus)
        currentMember.OperatorType = Operator::UnaryNegative;
    else if (operationType == ExpressionParserTreeSymbols::TerminalPlus)
        currentMember.OperatorType = Operator::UnaryPositive;
    else if (operationType == ExpressionParserTreeSymbols::TerminalOperatorHash)
        currentMember.OperatorType = Operator::UnaryImmediate;
    else if (operationType == ExpressionParserTreeSymbols::TerminalIncrement && !rightHandSideUnary)
        currentMember.OperatorType = Operator::UnaryPreIncrement;
    else if (operationType == ExpressionParserTreeSymbols::TerminalIncrement && rightHandSideUnary)
        currentMember.OperatorType = Operator::UnaryPostIncrement;
    else if (operationType == ExpressionParserTreeSymbols::TerminalDecrement && !rightHandSideUnary)
        currentMember.OperatorType = Operator::UnaryPreDecrement;
    else if (operationType == ExpressionParserTreeSymbols::TerminalDecrement && rightHandSideUnary)
        currentMember.OperatorType = Operator::UnaryPostDecrement;
}

}