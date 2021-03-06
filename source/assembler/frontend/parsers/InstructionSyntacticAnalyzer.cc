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
                else if (_symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::TerminalInstructionLD || 
                         _symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::TerminalInstructionJP ||
                         _symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::TerminalInstructionCALL) // TWO Operands
                {
                    opcodeType = TerminalToOpcode(_symbols[leftSubstring]);
                    instructionClass = InstructionClass::TwoOperands;

                    Shift(leftSubstring);
                    state = 2;
                }
                else if (_symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::TerminalInstructionINC || 
                         _symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::TerminalInstructionJP ||
                         _symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::TerminalInstructionCALL) // ONE Operands
                {
                    opcodeType = TerminalToOpcode(_symbols[leftSubstring]);
                    instructionClass = InstructionClass::OneOperand;

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
                if (_symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::TerminalIdentifier) // Variable, register, constant
                {
                    // Reduce Identifier
                    Shift(leftSubstring);
                    state = 4;
                }
                else if (_symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::TerminalNumericLiteral) // Number, string bool, etc.
                {
                    // Reduce Identifier
                    Shift(leftSubstring);
                    state = 7;
                }
                else if (_symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::TerminalOpenBrackets) // Expression
                {
                    // Reduce Expression
                    Shift(leftSubstring);
                    state = 11;
                }
                else if (instructionClass == InstructionClass::TwoOperands && 
                        (_symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::NonTerminalIdentifierOperand ||
                         _symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::NonTerminalNumericLiteralOperand))
                {
                    // detect Comma or one-operand version of the instruction
                    Shift(leftSubstring);
                    state = 5;   
                }
                else if (instructionClass == InstructionClass::OneOperand && 
                        (_symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::NonTerminalIdentifierOperand ||
                         _symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::NonTerminalNumericLiteralOperand))
                {
                    // Reduce One Operand instruction
                    Shift(leftSubstring);
                    state = 10;   
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == 5) // Detect Second Operand
            {
                if (leftSubstring >= static_cast<int>(_symbols.size()))
                {
                    if ((opcodeType == OpcodeType::jp && instructionClass == InstructionClass::TwoOperands) ||
                        (opcodeType == OpcodeType::call && instructionClass == InstructionClass::TwoOperands))
                    {
                        instructionClass = InstructionClass::OneOperand;
                        leftSubstring--;
                        // Reduce One Operand instruction
                        Shift(leftSubstring);
                        state = 10;   
                    }    
                    else
                    {
                        Reject(); break;
                    }
                }
                else if (_symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::TerminalComma)
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
                ReduceTwoOperandsInstruction(leftSubstring);
                break;
            }
            else if (state == 10) // Reduce Separator
            {
                ReduceOneOperandInstruction(leftSubstring);
                break;
            }
            else if (state == 11) // Reduce Expression
            {
                ReduceExpression(leftSubstring, beginIt, endIt);
                break;
            }
        }

        for (auto i : _symbols)
        {
            cout << '\t' << static_cast<size_t>(i.Symbol) << '\n';
        }
    }

    auto intermediateRepresentation = make_shared<InstructionIntermediateRepresentation>(_line, _column, 0llu, 0llu);
    return intermediateRepresentation;
}

void InstructionSyntacticAnalyzer::ReduceExpression(int leftSubstring, vector<Token>::iterator& beginIt, vector<Token>::iterator& endIt)
{
    ExpressionSyntacticAnalyzer expressionParser;

    try
    {
        auto beginIterator = beginIt + leftSubstring - 1;
        auto endIterator = endIt;
        auto intermediateRepresentation = expressionParser.TryToAccept(beginIterator, endIterator);
    }
    catch (SyntacticAnalyzerException e)
    {}
}

void InstructionSyntacticAnalyzer::ReduceTwoOperandsInstruction(int leftSubstring)
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

void InstructionSyntacticAnalyzer::ReduceOneOperandInstruction(int leftSubstring)
{
    // Remove Operand 1
    _symbols.erase(_symbols.begin() + leftSubstring - 1);
    // Remove Opcode
    _symbols.erase(_symbols.begin() + leftSubstring - 2);
    _symbols.insert(_symbols.begin() + leftSubstring - 2, { .Symbol = InstructionParserTreeSymbol::NonTerminalInstruction, .Lexeme = string("") });
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
        case InstructionParserTreeSymbol::TerminalInstructionINC: return OpcodeType::inc;
        case InstructionParserTreeSymbol::TerminalInstructionJP: return OpcodeType::jp;
        case InstructionParserTreeSymbol::TerminalInstructionCALL: return OpcodeType::call;
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
            case TokenType::SeparatorOPENBRACKETS: 
                return {.Symbol = InstructionParserTreeSymbol::TerminalOpenBrackets, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::LiteralNumericBINARY: 
            case TokenType::LiteralNumericOCTAL: 
            case TokenType::LiteralNumericDECIMAL: 
            case TokenType::LiteralNumericHEXADECIMAL: 
                return {.Symbol = InstructionParserTreeSymbol::TerminalNumericLiteral, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::InstructionMnemonicLD:
                return {.Symbol = InstructionParserTreeSymbol::TerminalInstructionLD, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::InstructionMnemonicINC:
                return {.Symbol = InstructionParserTreeSymbol::TerminalInstructionINC, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::InstructionMnemonicJP:
                return {.Symbol = InstructionParserTreeSymbol::TerminalInstructionJP, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::InstructionMnemonicCALL:
                return {.Symbol = InstructionParserTreeSymbol::TerminalInstructionCALL, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
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