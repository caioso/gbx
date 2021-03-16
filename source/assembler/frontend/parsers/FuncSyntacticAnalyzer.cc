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
                    state = 2;
                }
                else if (_symbols[leftSubstring].Symbol == FuncParseTreeSymbols::NonTerminalHeader) // Detect 'Header'
                {
                    Shift(leftSubstring);
                    state = 4;
                }
                else if (_symbols[leftSubstring].Symbol == FuncParseTreeSymbols::NonTerminalFunc) // Detect 'Non-terminal FUNC'
                {
                    Accept(); break;
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
                    state = 3;
                }
                else 
                {
                    Reject(); break;
                }
            }
            else if (state == 3) // Try to reduce [BGN found]
            {
                if (_symbols[leftSubstring].Symbol == FuncParseTreeSymbols::TerminalBgn ||
                    _symbols[leftSubstring].Symbol == FuncParseTreeSymbols::TerminalIn)
                {
                    // Reduce
                    // Remove Identifier
                    _symbols.erase(begin(_symbols) + leftSubstring - 1);
                    // Remove FUNC
                    _symbols.erase(begin(_symbols) + leftSubstring - 2);
                    _symbols.insert(begin(_symbols) +  leftSubstring - 2, { .Symbol = FuncParseTreeSymbols::NonTerminalHeader, .Lexeme = string("") });
                    break;
                }
                else 
                {
                    Reject(); break;
                }
            }
            else if (state == 4)
            {
                if (_symbols[leftSubstring].Symbol == FuncParseTreeSymbols::TerminalBgn) // 'BGN'
                {
                    Shift(leftSubstring);
                    state = 5;
                }
                else if (_symbols[leftSubstring].Symbol == FuncParseTreeSymbols::NonTerminalBody) // 'Body'
                {
                    // Reduce
                    // Remove Body
                    _symbols.erase(begin(_symbols) + leftSubstring);
                    // Remove Header
                    _symbols.erase(begin(_symbols) + leftSubstring - 1);
                    _symbols.push_back({ .Symbol = FuncParseTreeSymbols::NonTerminalFunc, .Lexeme = string("") });
                    break;
                }
                else if (_symbols[leftSubstring].Symbol == FuncParseTreeSymbols::TerminalIn) // Detect Argument
                {
                    Shift(leftSubstring);
                    state = 7;
                }
                else if (_symbols[leftSubstring].Symbol == FuncParseTreeSymbols::NonTerminalArgument)
                {
                    Shift(leftSubstring);
                    state = 10;
                }
                else if (_symbols[leftSubstring].Symbol == FuncParseTreeSymbols::NonTerminalArgumentList)
                {
                    Shift(leftSubstring);
                    state = 11;
                }
                else 
                {
                    Reject(); break;
                }
            }
            else if (state == 5) // skip the function body
            {
                if (_symbols[leftSubstring].Symbol == FuncParseTreeSymbols::TerminalIgnore)
                {
                    Shift(leftSubstring);
                    state = 5;
                }
                else if (_symbols[leftSubstring].Symbol == FuncParseTreeSymbols::TerminalEnd)
                {
                    Shift(leftSubstring);
                    state = 6;
                }
                else 
                {
                    Reject(); break;
                }
            }
            else if (state == 6) // Try to reduce [the function body]
            {
                if (_symbols[leftSubstring].Symbol == FuncParseTreeSymbols::TerminalEnd)
                {
                    auto functionBegin = find_if(_symbols.begin(), _symbols.end(), [&](const FuncCompoundSymbol& a) -> auto
                    {
                        return a.Symbol == FuncParseTreeSymbols::TerminalBgn;
                    });

                    // Reduce
                    // Remove Body
                    _symbols.erase(functionBegin, begin(_symbols) + leftSubstring);
                    // Add nonterminal body
                    _symbols.push_back({ .Symbol = FuncParseTreeSymbols::NonTerminalBody, .Lexeme = string("") });

                    cout << "After removing the body" << '\n';
                    for (auto symbol : _symbols)
                        cout << static_cast<size_t>(symbol.Symbol) << " -> " << symbol.Lexeme << '\n';
                    break;
                }
                else 
                {
                    Reject(); break;
                }
            }
            else if (state == 7)
            {
                if (_symbols[leftSubstring].Symbol == FuncParseTreeSymbols::TerminalSemicolon)
                {
                    Shift(leftSubstring);
                    state = 8;
                }
                else
                {
                    Reject(); break;
                }
            }        
            else if (state == 8)
            {
                if (_symbols[leftSubstring].Symbol == FuncParseTreeSymbols::TerminalIdentifier)
                {
                    Shift(leftSubstring);
                    state = 9;
                }
                else
                {
                    Reject(); break;
                }
            }        
            else if (state == 9) // detect Identifier
            {
                if (_symbols[leftSubstring].Symbol == FuncParseTreeSymbols::TerminalBgn ||
                    _symbols[leftSubstring].Symbol == FuncParseTreeSymbols::TerminalIn)
                {
                    cout << "Reduce argument" << '\n';

                    // Reduce
                    // Remove identifier
                    _symbols.erase(begin(_symbols) + leftSubstring - 1);
                    // Remove Colon
                    _symbols.erase(begin(_symbols) + leftSubstring - 2);
                    // Remove IN
                    _symbols.erase(begin(_symbols) + leftSubstring - 3);
                    // Add nonterminal body
                    _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = FuncParseTreeSymbols::NonTerminalArgument, .Lexeme = string("") });
                    break;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == 10) // Detect Argument
            {
                if (_symbols[leftSubstring].Symbol == FuncParseTreeSymbols::TerminalBgn || 
                    _symbols[leftSubstring].Symbol == FuncParseTreeSymbols::TerminalIn)
                {
                    cout << "Reduce Non Terminal arg into list" << '\n';
                    // Reduce
                    // Remove Argument
                    _symbols.erase(begin(_symbols) + leftSubstring - 1);
                    // Add Argument List
                    _symbols.insert(begin(_symbols) +  leftSubstring - 1, { .Symbol = FuncParseTreeSymbols::NonTerminalArgumentList, .Lexeme = string("") });
                    break;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == 11) // Detect Argument List
            {
                if (_symbols[leftSubstring].Symbol == FuncParseTreeSymbols::TerminalBgn)
                {
                    Shift(leftSubstring);
                    state = 5;
                }
                else if (_symbols[leftSubstring].Symbol == FuncParseTreeSymbols::NonTerminalArgument)
                {
                    cout << "Combine argument with the argment list" << '\n';
                    // Reduce
                    // Remove Argument (only argument list remain)
                    _symbols.erase(begin(_symbols) + leftSubstring);

                    for (auto symbol : _symbols)
                        cout << static_cast<size_t>(symbol.Symbol) << " -> " << symbol.Lexeme << '\n';
                    break;
                }
                else if (_symbols[leftSubstring].Symbol == FuncParseTreeSymbols::TerminalIn)
                {
                    Shift(leftSubstring);
                    state = 7;
                }
                else if (_symbols[leftSubstring].Symbol == FuncParseTreeSymbols::NonTerminalBody)
                {
                    cout << "Non Terminal Body Detected, with args list" << '\n';
                    // Reduce
                    // Remove Body
                    _symbols.erase(begin(_symbols) + leftSubstring);
                    // Remove ArguementList
                    _symbols.erase(begin(_symbols) + leftSubstring - 2);
                    // Remove Header
                    _symbols.erase(begin(_symbols) + leftSubstring - 3);
                    _symbols.push_back({ .Symbol = FuncParseTreeSymbols::NonTerminalFunc, .Lexeme = string("") });
                    break;
                }
                else
                {
                    Reject(); break;
                }
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
    auto bgnFound = false;

    // (functionEnd - 1) because only the last END must be converted to 'TerminalEnd'
    // Anything in between becomes 'TerminalIgnore'. Thats why TokenType::KeywordEND is not in the switch block.
    transform(beginIt, (functionEnd - 1), back_inserter(_symbols), [&](Token x) -> FuncCompoundSymbol
    {
        if (bgnFound && x.Type != TokenType::KeywordEND)
            return {.Symbol = FuncParseTreeSymbols::TerminalIgnore, .Lexeme  = x.Lexeme };

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
                bgnFound = true; // If bgn has been found, the body of the function started. Ignore everything up to the target end.
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