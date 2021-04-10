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
        size_t state = 0;
        ExpressionParserTreeSymbols operationType;

        while(true)
        {
            // Think of how to deal with incomplete parsing and the end of the input string.
            if (state == 0)
            {
                leftSubstring++;
                
                if (_symbols.size() == 1 && _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalExpression) 
                {Accept(); break;}

                state = 1;
                
            }
            else if (state == 1)
            {
                auto resultingOperation = EvaluateOperand(leftSubstring, state);

                if (resultingOperation == NextOperation::Rejected)
                { Reject(); break; }
                else if (resultingOperation == NextOperation::Accepted)
                { Accept(); break; }
                else if (resultingOperation == NextOperation::Reduced)
                    break;
            }
            // Further evaluate operation
            else if (state == 2)
            {
                // Move further ( Should remember that + has been parsed)
                if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalPlus ||
                    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalMinus ||
                    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalMultiplication ||
                    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalDivision ||
                    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalBitwiseAnd ||
                    _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalBitwiseOr)
                {
                    operationType = _symbols[leftSubstring].Symbol;
                    leftSubstring++;
                    state = 3;
                }
                else if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalCloseParenthesis)
                {
                    // Try to reduce (E) -> E6
                    if (_symbols[leftSubstring - 1].Symbol == ExpressionParserTreeSymbols::NonTerminalExpression &&
                        _symbols[leftSubstring - 2].Symbol == ExpressionParserTreeSymbols::TerminalOpenParenthesis)
                    {
                        // Reduce Surrounded operation
                        // Remove )
                        _symbols.erase(begin(_symbols) + leftSubstring);
                        // Remove E
                        _symbols.erase(begin(_symbols) + leftSubstring - 1);
                        // Remove (
                        _symbols.erase(begin(_symbols) + leftSubstring - 2);
                        _symbols.insert(begin(_symbols) +  leftSubstring - 2, { .Symbol = ExpressionParserTreeSymbols::NonTerminalSurroundedOperation, .Lexeme = string("") });
                        break;
                    }
                    else
                    {
                        cout << "Rejected State 2" << '\n';
                        Reject(); break;
                    }
                }
                else 
                {
                    cout << "Rejected State 2" << '\n';
                    Reject(); break;
                }
            }
            // Further evaluate operation
            else if (state == 3)
            {
                auto resultingOperation = EvaluateOperand(leftSubstring, state);

                if (resultingOperation == NextOperation::Rejected)
                { Reject(); break; }
                else if (resultingOperation == NextOperation::Accepted)
                { Accept(); break; }
                else if (resultingOperation == NextOperation::Reduced)
                    break;
            }
            // Reduce expression of type E + E
            else if (state == 4)
            {
                cout << "Reduce binary op" << '\n';
                // Reduce Binary operation E + E -> F1
                // Remove Identifier
                _symbols.erase(begin(_symbols) + leftSubstring - 1);
                // Remove +
                _symbols.erase(begin(_symbols) + leftSubstring - 2);
                // Remove Identifier
                _symbols.erase(begin(_symbols) + leftSubstring - 3);

                if (operationType == ExpressionParserTreeSymbols::TerminalPlus)
                    _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryAddition, .Lexeme = string("") });
                else if (operationType == ExpressionParserTreeSymbols::TerminalMinus)
                    _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinarySubtraction, .Lexeme = string("") });
                else if (operationType == ExpressionParserTreeSymbols::TerminalMultiplication)
                    _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryMultiplication, .Lexeme = string("") });
                else if (operationType == ExpressionParserTreeSymbols::TerminalDivision)
                    _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryDivision, .Lexeme = string("") });
                else if (operationType == ExpressionParserTreeSymbols::TerminalBitwiseAnd)
                    _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryBitwiseAnd, .Lexeme = string("") });
                else if (operationType == ExpressionParserTreeSymbols::TerminalBitwiseOr)
                    _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryBitwiseOr, .Lexeme = string("") });
                break;
            }
        }

        cout << "Size: " << _symbols.size() << '\n';
        for_each(_symbols.begin(), _symbols.end(), [](ExpressionCompoundSymbol x) -> auto
        {
            cout << static_cast<size_t>(x.Symbol) << " -> " << x.Lexeme << '\n';
        });
        cout << '\n';
    }

    auto representation = make_shared<ExpressionIntermediateRepresentation>(_expressionStack, _line, _column);
    return representation;
}

NextOperation ExpressionSyntacticAnalyzer::EvaluateOperand(int& leftSubstring, size_t& state)
{
    if (leftSubstring >= static_cast<int>(_symbols.size()))
        return NextOperation::Rejected; 

    // NON TERMINALS
    // Detect Identifier
    if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalIdentifier)
    {
        // Reduce Identifier to Non-Terminal Identifier (T4)
        // Remove Identifier
        _symbols.erase(begin(_symbols) + leftSubstring);
        _symbols.insert(begin(_symbols) + leftSubstring, { .Symbol = ExpressionParserTreeSymbols::NonTerminalIdentifier, .Lexeme = string("") });
        return NextOperation::Reduced;
    }
    // Detect Open Parenthesis
    else if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalOpenParenthesis)
    {
        // Do nothing, simply shift
        leftSubstring++;
        state = 1;
        return NextOperation::Shift;
    }
    // TERMINALS
    else if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalIdentifier)
    {
        // Reduce Identifier to Non-Terminal Operand (E6)
        // Remove Non-terminal Identifier
        _symbols.erase(begin(_symbols) + leftSubstring);
        _symbols.insert(begin(_symbols) + leftSubstring, { .Symbol = ExpressionParserTreeSymbols::NonTerminalOperand, .Lexeme = string("") });
        return NextOperation::Reduced;
    }
    else if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryAddition ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinarySubtraction ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryMultiplication ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryDivision ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryBitwiseAnd ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryBitwiseOr)
    {
        // Reduce Identifier to Non-Terminal Binary Operation (F1/F2/F3/F4/F5/F6) to Non Terminal Operation (E7)
        // Remove Non-terminal Operand
        _symbols.erase(begin(_symbols) + leftSubstring);
        _symbols.insert(begin(_symbols) + leftSubstring, { .Symbol = ExpressionParserTreeSymbols::NonTerminalOperation, .Lexeme = string("") });
        return NextOperation::Reduced;
    }
    else if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalOperand ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalOperation || 
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalSurroundedOperation)
    {
        // Reduce Identifier to Non-Terminal Expression (E)
        // Remove Non-terminal Operand
        _symbols.erase(begin(_symbols) + leftSubstring);
        _symbols.insert(begin(_symbols) + leftSubstring, { .Symbol = ExpressionParserTreeSymbols::NonTerminalExpression, .Lexeme = string("") });
        return NextOperation::Reduced;
    }

    // NON TERMINAL EXPRESSION
    else if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalExpression)
    {
        // 'First part of a binary operation'
        if (state == 1)
            state = 2;
        // 'Second part of a binary operation'
        else if (state == 3)
            state = 4;
        
        leftSubstring++;
        return NextOperation::Shift;
    }
    else 
    {
        cout << "Rejected" << '\n';
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
            case TokenType::SeparatorOPENPARENTHESIS:
                return {.Symbol = ExpressionParserTreeSymbols::TerminalOpenParenthesis, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::SeparatorCLOSEPARENTHESIS:
                return {.Symbol = ExpressionParserTreeSymbols::TerminalCloseParenthesis, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
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
        }
    });

    _line = (*beginIt).Line;
    _column = (*beginIt).Column;

    for_each(_symbols.begin(), _symbols.end(), [](ExpressionCompoundSymbol x) -> auto
    {
        cout << static_cast<size_t>(x.Symbol) << " -> " << x.Lexeme << '\n';
    });
}

}