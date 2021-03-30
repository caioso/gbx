#include "ExpressionSyntacticAnalyzer.h"

using namespace gbxasm;
using namespace gbxasm::intermediate_representation;
using namespace std;

namespace gbxasm::frontend::parsers
{

shared_ptr<IntermediateRepresentation> ExpressionSyntacticAnalyzer::TryToAccept(vector<Token>::iterator& beginIt, vector<Token>::iterator& endIt)
{
    ExtractSymbols(beginIt, endIt);

    while (!IsAccepted() && !IsRejected() && !IsExited())
    {        
        auto leftSubstring = -1; 
        auto state = 0;

        while(true)
        {
            if (state == 0)
            {
                leftSubstring++;
                state = 1;
            }
            else if (state == 1) 
            // For expressions like (A <OP> B)
            //  - Detect first operand (identifier)
            //  - Detect first operand (numeric literal)
            {
                // Detection of non-terminals happens here
                if (IsOutOfBonds(leftSubstring, _symbols.size())) { Reject(); break; }

                if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalExpression)
                {
                    Accept(); break;
                }
                else if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryExpression)
                {
                    leftSubstring++;
                    state = 5;
                }
                else if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalIdentifier ||
                         _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalNumericLiteral)
                {
                    PushIdentifier(_symbols[leftSubstring]);
                    leftSubstring++;
                    state = 2;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == 2)
            // For expressions like (A <OP> B)
            //  - Detect operator
            {
                if (IsOutOfBonds(leftSubstring, _symbols.size())) { Reject(); break; }

                if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalBinaryOperatorPlus ||
                    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalBinaryOperatorMinus ||
                    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalBinaryOperatorMultiplication ||
                    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalBinaryOperatorDivision ||
                    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalBinaryOperatorRightShift ||
                    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalBinaryOperatorLeftShift ||
                    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalBinaryOperatorLessThan ||
                    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalBinaryOperatorLessThanEqual)
                {
                    PushBinaryOperator(_symbols[leftSubstring]);
                    leftSubstring++;
                    state = 3;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == 3)
            // For expressions like (A <OP> B)
            //  - Detect second operand (identifier)
            //  - Detect second operand (numeric Literal)
            {
                if (IsOutOfBonds(leftSubstring, _symbols.size())) { Reject(); break; }

                if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalIdentifier ||
                    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalNumericLiteral)
                {
                    PushIdentifier(_symbols[leftSubstring]);
                    leftSubstring++;
                    state = 4;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == 4)
            // Reduce Expression Like (A <OP> B)
            {
                ReduceBinaryExpression(leftSubstring);
                leftSubstring++;
                break;
            }
            else if (state == 5)
            {
                // reach the end of the symbols array
                if (_symbols.begin() + leftSubstring == _symbols.end())
                {
                    ReduceExpression(leftSubstring);
                    break;
                }
                else
                {
                    Reject(); break;
                }
            }
        }
    }

    auto representation = make_shared<ExpressionIntermediateRepresentation>(_expressionStack, _line, _column);
    return representation;
}

void ExpressionSyntacticAnalyzer::PushIdentifier(ExpressionCompoundSymbol symbol)
{
    ExpressionMember member;
    member.Lexeme = symbol.Lexeme;
    _expressionStack.push(member);
}

void ExpressionSyntacticAnalyzer::PushBinaryOperator(ExpressionCompoundSymbol symbol)
{   
    ExpressionMember member;
    member.Lexeme = symbol.Lexeme;
    _expressionStack.push(member);
}

void ExpressionSyntacticAnalyzer::ReduceExpression(int leftSubstring)
{
    // Reduce
    // Remove previous expression
    _symbols.erase(begin(_symbols) + leftSubstring - 1);
    // Add nonterminal argument
    _symbols.insert(begin(_symbols) +  leftSubstring - 1, { .Symbol = ExpressionParserTreeSymbols::NonTerminalExpression, .Lexeme = string("") });

    cout << "After reducing at the end of the string" << '\n';
    for_each(_symbols.begin(), _symbols.end(), [](ExpressionCompoundSymbol x) -> auto
    {
        cout << static_cast<size_t>(x.Symbol) << " -> " << x.Lexeme << '\n';
    });
}

void ExpressionSyntacticAnalyzer::ReduceBinaryExpression(int leftSubstring)
{
    // Reduce
    // Remove identifier
    _symbols.erase(begin(_symbols) + leftSubstring - 1);
    // Remove Binary Operator
    _symbols.erase(begin(_symbols) + leftSubstring - 2);
    // Remove identifier
    _symbols.erase(begin(_symbols) + leftSubstring - 3);
    // Add nonterminal argument
    _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryExpression, .Lexeme = string("") });

    cout << "After reducing binary expressionm" << '\n';
    for_each(_symbols.begin(), _symbols.end(), [](ExpressionCompoundSymbol x) -> auto
    {
        cout << static_cast<size_t>(x.Symbol) << " -> " << x.Lexeme << '\n';
    });
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
            case TokenType::OperatorPLUS:
                return {.Symbol = ExpressionParserTreeSymbols::TerminalBinaryOperatorPlus, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorMINUS:
                return {.Symbol = ExpressionParserTreeSymbols::TerminalBinaryOperatorMinus, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorMULTIPLICATION:
                return {.Symbol = ExpressionParserTreeSymbols::TerminalBinaryOperatorMultiplication, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorDIVISION:
                return {.Symbol = ExpressionParserTreeSymbols::TerminalBinaryOperatorDivision, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorRIGHTSHIFT:
                return {.Symbol = ExpressionParserTreeSymbols::TerminalBinaryOperatorRightShift, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorLEFTSHIFT:
                return {.Symbol = ExpressionParserTreeSymbols::TerminalBinaryOperatorLeftShift, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorLESSTHAN:
                return {.Symbol = ExpressionParserTreeSymbols::TerminalBinaryOperatorLessThan, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorLESSTHANOREQUALTO:
                return {.Symbol = ExpressionParserTreeSymbols::TerminalBinaryOperatorLessThanEqual, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::LiteralNumericBINARY:
            case TokenType::LiteralNumericOCTAL:
            case TokenType::LiteralNumericDECIMAL:
            case TokenType::LiteralNumericHEXADECIMAL:
                return {.Symbol = ExpressionParserTreeSymbols::TerminalNumericLiteral, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            default:
                return {.Symbol = ExpressionParserTreeSymbols::TerminalIgnore, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
        }
    });

    auto firstValid = find_if(_symbols.begin(), _symbols.end(), [](ExpressionCompoundSymbol x) -> auto
    {
        if (x.Symbol != ExpressionParserTreeSymbols::TerminalIgnore)
            return true;
        return false;
    });

    _line = (*firstValid).Line;
    _column = (*firstValid).Column;

    for_each(_symbols.begin(), _symbols.end(), [](ExpressionCompoundSymbol x) -> auto
    {
        cout << static_cast<size_t>(x.Symbol) << " -> " << x.Lexeme << '\n';
    });
}

}