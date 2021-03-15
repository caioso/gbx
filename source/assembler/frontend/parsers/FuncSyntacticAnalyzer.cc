#include "FuncSyntacticAnalyzer.h"

using namespace gbxasm;
using namespace gbxasm::frontend;
using namespace std;

namespace gbxasm::frontend::parsers
{

shared_ptr<gbxasm::intermediate_representation::IntermediateRepresentation> FuncSyntacticAnalyzer::TryToAccept(vector<Token>::iterator& beginIt, vector<Token>::iterator& endIt)
{
    ExtractSymbols(beginIt, endIt);

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
            else if (state == 1) // Detect 'FUNC'
            {
                if (_symbols[leftSubstring].Symbol == FuncParseTreeSymbols::TerminalFunc)
                {
                    Shift(leftSubstring);
                    state++;
                }
                else 
                {
                    Reject(); break;
                }
            }
            else if (state == 2) // Detect function's identifier
            {
                if (_symbols[leftSubstring].Symbol == FuncParseTreeSymbols::TerminalIdentifier)
                {
                    Shift(leftSubstring);
                    state++;
                }
                else 
                {
                    Reject(); break;
                }
            }
            else if (state == 3) // Try to reduce 
            {
                Accept();
                return{};
            }
            else 
            {
                Reject(); break;
            }
        }
    }

    return {};
}

void FuncSyntacticAnalyzer::ExtractSymbols(vector<Token>::iterator& beginIt, vector<Token>::iterator& endIt)
{
    auto functionEnd = beginIt + CountEndWithinFunctionBody(beginIt, endIt);

    // (functionEnd - 1) because only the last END must be converted to 'TerminalEnd'
    // Anything in between becomes 'TerminalIgnore'. Thats why TokenType::KeywordEND is not in the switch block.
    transform(beginIt, (functionEnd - 1), back_inserter(_symbols), [](Token x) -> FuncCompoundSymbol
    {
        switch (x.Type)
        {
            case TokenType::KeywordFUNC: 
                return {.Symbol = FuncParseTreeSymbols::TerminalFunc, .Lexeme  = x.Lexeme };
            case TokenType::Identifier: 
                return {.Symbol = FuncParseTreeSymbols::TerminalIdentifier, .Lexeme  = x.Lexeme };
            case TokenType::KeywordIN: 
                return {.Symbol = FuncParseTreeSymbols::TerminalIn, .Lexeme  = x.Lexeme };
            case TokenType::OperatorSEMICOLON:
                return {.Symbol = FuncParseTreeSymbols::TerminalSemicolon, .Lexeme  = x.Lexeme };
            case TokenType::KeywordOUT:
                return {.Symbol = FuncParseTreeSymbols::TerminalOut, .Lexeme  = x.Lexeme };
            case TokenType::KeywordAS:
                return {.Symbol = FuncParseTreeSymbols::TerminalAs, .Lexeme  = x.Lexeme };
            case TokenType::KeywordBYTE:
            case TokenType::KeywordWORD:
            case TokenType::KeywordDWRD:
            case TokenType::KeywordSTR:
            case TokenType::KeywordCHAR:
            case TokenType::KeywordBOOL:
                 return {.Symbol = FuncParseTreeSymbols::TerminalType, .Lexeme  = x.Lexeme };
            case TokenType::SeparatorOPENBRACKETS: 
                return {.Symbol = FuncParseTreeSymbols::TerminalOpenBracket, .Lexeme  = x.Lexeme };
            case TokenType::SeparatorCLOSEBRACKETS: 
                return {.Symbol = FuncParseTreeSymbols::TerminalCloseBracket, .Lexeme  = x.Lexeme };
            case TokenType::LiteralNumericDECIMAL:
            case TokenType::LiteralNumericHEXADECIMAL:
            case TokenType::LiteralNumericOCTAL:
            case TokenType::LiteralNumericBINARY:
                return {.Symbol = FuncParseTreeSymbols::TerminalOpenNumericLiteral, .Lexeme  = x.Lexeme };
             case TokenType::KeywordBGN: 
                return {.Symbol = FuncParseTreeSymbols::TerminalBgn, .Lexeme  = x.Lexeme };
            default:
                return {.Symbol = FuncParseTreeSymbols::TerminalIgnore, .Lexeme  = x.Lexeme };
        }
    });

    if ((*(functionEnd - 1)).Type == TokenType::KeywordEND)
    {
        FuncCompoundSymbol end = {.Symbol = FuncParseTreeSymbols::TerminalEnd, .Lexeme  = (*(functionEnd - 1)).Lexeme };
        _symbols.emplace_back(end);
    }
    else
    {
        // Something when wrong.
    }

    for (auto symbol : _symbols)
        cout << static_cast<size_t>(symbol.Symbol) << " -> " << symbol.Lexeme << '\n';
}

size_t FuncSyntacticAnalyzer::CountEndWithinFunctionBody(vector<Token>::iterator& beginIt, vector<Token>::iterator& endIt)
{
    auto counter = 0;
    auto position = 0;
    for (auto it = beginIt; it != endIt; ++it)
    {
        if ((*it).Type == TokenType::KeywordFUNC || (*it).Type == TokenType::KeywordREPT || 
            (*it).Type == TokenType::KeywordWHEN || (*it).Type == TokenType::KeywordTRY ||
            (*it).Type == TokenType::KeywordIF ||  (*it).Type == TokenType::KeywordPACK)
            counter++;

        if ((*it).Type == TokenType::KeywordEND)
            counter--;

        position++;

        if (counter == 0)
            break;
    }

    return position;
}

inline void FuncSyntacticAnalyzer::Shift(int& top)
{
    top++;
}

}