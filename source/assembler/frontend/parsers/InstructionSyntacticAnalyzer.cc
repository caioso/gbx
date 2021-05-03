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
                if (_symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::TerminalInstructionLD)
                {
                    opcodeType = TerminalToOpcode(_symbols[leftSubstring]);
                    instructionClass = InstructionClass::TwoOperands;

                    Shift(leftSubstring);
                    state = 2;
                }
                else if (_symbols[leftSubstring].Symbol == InstructionParserTreeSymbol::NonTerminaOpcode)
                {
                    Shift(leftSubstring);
                    state = 3;
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
            else if (state == 3) // Start detecting operands
            {
                cout << "Detecting operands" << '\n';
                Reject(); break;
            }
        }
    }

    auto intermediateRepresentation = make_shared<InstructionIntermediateRepresentation>(_line, _column);
    return intermediateRepresentation;
}

void InstructionSyntacticAnalyzer::ReduceOpcode(int leftSubstring)
{
    _symbols.erase(_symbols.begin() + leftSubstring - 1);
    _symbols.insert(_symbols.begin() + leftSubstring - 1, { .Symbol = InstructionParserTreeSymbol::NonTerminaOpcode, .Lexeme = string("") });
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