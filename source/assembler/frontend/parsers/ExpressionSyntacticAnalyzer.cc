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
                if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalPlus)
                {
                    leftSubstring++;
                    state = 3;
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
                _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = ExpressionParserTreeSymbols::NonTerminalBinaryAddition, .Lexeme = string("") });
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
    // NON TERMINALS
    // Detect Identifier
    if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::TerminalIdentifier)
    {
        // Reduce Identifier to Non-Terminal Identifier (T4)
        // Remove Identifier
        _symbols.erase(begin(_symbols) + leftSubstring);
        _symbols.insert(begin(_symbols) +  leftSubstring, { .Symbol = ExpressionParserTreeSymbols::NonTerminalIdentifier, .Lexeme = string("") });
        return NextOperation::Reduced;
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
    else if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalBinaryAddition)
    {
        // Reduce Identifier to Non-Terminal Binary Addition (F1) to Non Terminal Operation (E7)
        // Remove Non-terminal Operand
        _symbols.erase(begin(_symbols) + leftSubstring);
        _symbols.insert(begin(_symbols) + leftSubstring, { .Symbol = ExpressionParserTreeSymbols::NonTerminalOperation, .Lexeme = string("") });
        return NextOperation::Reduced;
    }
    else if (_symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalOperand ||
             _symbols[leftSubstring].Symbol == ExpressionParserTreeSymbols::NonTerminalOperation)
    {
        // Reduce Identifier to Non-Terminal Expression (E)
        // Remove Non-terminal Operand
        _symbols.erase(begin(_symbols) + leftSubstring);
        _symbols.insert(begin(_symbols) +  leftSubstring, { .Symbol = ExpressionParserTreeSymbols::NonTerminalExpression, .Lexeme = string("") });
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
            case TokenType::OperatorPLUS: 
                return {.Symbol = ExpressionParserTreeSymbols::TerminalPlus, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
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