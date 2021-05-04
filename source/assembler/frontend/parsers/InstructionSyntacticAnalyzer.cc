#include "InstructionSyntacticAnalyzer.h"

using namespace std;
using namespace gbxasm::intermediate_representation;
using namespace gbxcore::instructions;

namespace gbxasm::frontend::parsers
{

shared_ptr<IntermediateRepresentation> InstructionSyntacticAnalyzer::TryToAccept(vector<Token>::iterator& beginIt, vector<Token>::iterator& endIt)
{
    ExtractSymbols(beginIt, endIt);
    OpcodeType opcodeType;
    InstructionClass instructionClass;

    while (!IsAccepted() && !IsRejected())
    {        
        auto leftSubstring = -1; 
        auto state = 0;

        while(true)
        {
            if (state == 0)
            {
                Shift(leftSubstring);
                state++;
            }
            else if (state == 1)
            {
                if (_symbols.size() == 1llu && _symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::NonTerminalInstruction)
                {
                    Accept();
                    break;
                }
                else if (_symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::TerminalInstructionLD)
                {
                    opcodeType = TerminalToOpcode(_symbols[leftSubstring]);
                    instructionClass = InstructionClass::TwoOperands;

                    Shift(leftSubstring);
                    state = 2;
                }
                else if (_symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::NonTerminalOpcode)
                {
                    Shift(leftSubstring);
                    state = 3;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == 3) // Start detecting operands
            {
                if (_symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::TerminalIdentifier)
                {
                    // Reduce Identifier
                    Shift(leftSubstring);
                    state = 4;
                }
                else if (_symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::TerminalNumericLiteral)
                {
                    // Reduce Identifier
                    Shift(leftSubstring);
                    state = 7;
                }
                else if (_symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::NonTerminalIdentifierOperand ||
                         _symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::NonTerminalNumericLiteralOperand)
                {
                    // detect Comma
                    Shift(leftSubstring);
                    state = 5;   
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == 5) // Detect Second Operand
            {
                if (_symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::TerminalComma)
                {
                    // detect Comma
                    Shift(leftSubstring);
                    state = 9;
                }
                else if (_symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::NonTerminalSeparator)
                {
                    Shift(leftSubstring);
                    state = 6;   
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == 6) // Start detecting second operands
            {
                if (_symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::TerminalIdentifier)
                {
                    // Reduce Identifier
                    Shift(leftSubstring);
                    state = 4;
                }
                else if (_symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::TerminalNumericLiteral)
                {
                    // Reduce Identifier
                    Shift(leftSubstring);
                    state = 7;
                }
                else if (_symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::NonTerminalNumericLiteralOperand ||
                         _symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::NonTerminalIdentifierOperand)
                {
                    // Reduce Identifier
                    Shift(leftSubstring);
                    state = 8;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == 2) // Reduce Opcode
            {
                ReduceOpcode(leftSubstring);
                break;
            }
            else if (state == 4) // Reduce Identifier as Operand
            {
                ReduceIdentifierOperand(leftSubstring);
                break;
            }
            else if (state == 7) // Reduce Numeric Literal Operand
            {
                ReduceNumericLiteralOperand(leftSubstring);
                break;
            }
            else if (state == 9) // Reduce Separator
            {
                ReduceSeparator(leftSubstring);
                break;
            }
            else if (state == 8) // Reduce Instruction
            {
                ReduceInstruction(leftSubstring);
                break;
            }
        }

        for (auto i : _symbols)
        {
            cout << '\t' << static_cast<size_t>(i.Symbol) << '\n';
        }
    }

    auto intermediateRepresentation = make_shared<InstructionIntermediateRepresentation>(_line, _column);
    return intermediateRepresentation;
}

void InstructionSyntacticAnalyzer::ReduceInstruction(int leftSubstring)
{
    // Remove Operand 2
    _symbols.erase(_symbols.begin() + leftSubstring - 1);
    // Remove Comma
    _symbols.erase(_symbols.begin() + leftSubstring - 2);
    // Remove Operand 1
    _symbols.erase(_symbols.begin() + leftSubstring - 3);
    // Remove Opcode
    _symbols.erase(_symbols.begin() + leftSubstring - 4);
    _symbols.insert(_symbols.begin() + leftSubstring - 4, { .Symbol = InstructionParserTreeSymbol::NonTerminalInstruction, .Lexeme = string("") });
}

void InstructionSyntacticAnalyzer::ReduceNumericLiteralOperand(int leftSubstring)
{
    _symbols.erase(_symbols.begin() + leftSubstring - 1);
    _symbols.insert(_symbols.begin() + leftSubstring - 1, { .Symbol = InstructionParserTreeSymbol::NonTerminalNumericLiteralOperand, .Lexeme = string("") });
}

void InstructionSyntacticAnalyzer::ReduceSeparator(int leftSubstring)
{
    _symbols.erase(_symbols.begin() + leftSubstring - 1);
    _symbols.insert(_symbols.begin() + leftSubstring - 1, { .Symbol = InstructionParserTreeSymbol::NonTerminalSeparator, .Lexeme = string("") });
}

void InstructionSyntacticAnalyzer::ReduceIdentifierOperand(int leftSubstring)
{
    _symbols.erase(_symbols.begin() + leftSubstring - 1);
    _symbols.insert(_symbols.begin() + leftSubstring - 1, { .Symbol = InstructionParserTreeSymbol::NonTerminalIdentifierOperand, .Lexeme = string("") });
}

void InstructionSyntacticAnalyzer::ReduceOpcode(int leftSubstring)
{
    _symbols.erase(_symbols.begin() + leftSubstring - 1);
    _symbols.insert(_symbols.begin() + leftSubstring - 1, { .Symbol = InstructionParserTreeSymbol::NonTerminalOpcode, .Lexeme = string("") });
}

OpcodeType InstructionSyntacticAnalyzer::TerminalToOpcode(InstructionCompoundSymbol compoundSymbol)
{
    switch (compoundSymbol.Symbol)
    {
        case InstructionParserTreeSymbol::TerminalInstructionLD: return OpcodeType::ld;
        default:
            stringstream ss;
            ss << "Invalid opcode '" << compoundSymbol.Lexeme << "'";
            throw  SyntacticAnalyzerException(ss.str());
    }
}

inline void InstructionSyntacticAnalyzer::Shift(int& leftSubstring)
{
    leftSubstring++;
}

void InstructionSyntacticAnalyzer::ExtractSymbols(vector<Token>::iterator& beginIt, vector<Token>::iterator& endIt)
{
    if (beginIt == endIt)
        return;

    // Only accept operators and identifiers for now (will be expanded with literals soon)
    transform(beginIt, endIt, back_inserter(_symbols), [&](Token x) -> InstructionCompoundSymbol
    {
        switch (x.Type)
        {
            case TokenType::Identifier: 
                return {.Symbol = InstructionParserTreeSymbol::TerminalIdentifier, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::SeparatorCOMMA: 
                return {.Symbol = InstructionParserTreeSymbol::TerminalComma, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::LiteralNumericBINARY: 
            case TokenType::LiteralNumericOCTAL: 
            case TokenType::LiteralNumericDECIMAL: 
            case TokenType::LiteralNumericHEXADECIMAL: 
                return {.Symbol = InstructionParserTreeSymbol::TerminalNumericLiteral, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::InstructionMnemonicLD:
                return {.Symbol = InstructionParserTreeSymbol::TerminalInstructionLD, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            default:
            {
                stringstream ss;
                ss << "Unexpected '" << x.Lexeme << "' found";
                throw SyntacticAnalyzerException(ss.str());
            }
        }
    });

    if (_symbols.size() != 0)
    {
        _line = _symbols[0].Line;
        _column = _symbols[0].Column;
    }
}

}