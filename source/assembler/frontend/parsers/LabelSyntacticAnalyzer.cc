#include "LabelSyntacticAnalyzer.h"

using namespace std;
using namespace gbxasm::intermediate_representation;

namespace gbxasm::frontend::parsers
{

shared_ptr<IntermediateRepresentation> LabelSyntacticAnalyzer::TryToAccept(vector<Token>::iterator& beginIt, vector<Token>::iterator& endIt)
{
    auto identifier = string("");
    auto scope = LabelScope::Local;
    ExtractSymbols(beginIt, endIt);

    while (!IsAccepted() && !IsRejected())
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
            {
                if (_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::TerminalIdentifier)
                {
                    leftSubstring++;
                    state = 2;
                }
                else if(_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::NonTerminalLabelName)
                {
                    leftSubstring++;
                    state = 3;
                }
                else if(_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::NonTerminalSimpleLabel ||
                        _symbols[leftSubstring].Symbol == LabelParserTreeSymbol::NonTerminalGloballyDefinedLabel ||
                        _symbols[leftSubstring].Symbol == LabelParserTreeSymbol::NonTerminalLocallyDefinedLabel)
                {
                    leftSubstring++;
                    state = 4;
                }
                else if (_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::NonTerminalLabel)
                {
                    Accept();
                    break;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == 2)
            {
                if (_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::TerminalSemiColon || 
                    _symbols[leftSubstring].Symbol == LabelParserTreeSymbol::TerminalLessThan)
                {
                    identifier = (*(begin(_symbols) + leftSubstring - 1)).Lexeme;

                    // Reduce
                    // Remove SemiColon
                    _symbols.erase(begin(_symbols) + leftSubstring - 1);
                    _symbols.insert(begin(_symbols) +  leftSubstring - 1, { .Symbol = LabelParserTreeSymbol::NonTerminalLabelName, .Lexeme = string("") });
                    break;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == 3)
            {
                if (_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::TerminalSemiColon)
                {
                    // Reduce
                    // Remove SemiColon
                    _symbols.erase(begin(_symbols) + leftSubstring);
                    // Remove Identifier Name
                    _symbols.erase(begin(_symbols) + leftSubstring - 1);
                    _symbols.insert(begin(_symbols) +  leftSubstring - 1, { .Symbol = LabelParserTreeSymbol::NonTerminalSimpleLabel, .Lexeme = string("") });
                    break;
                }
                else if (_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::NonTerminalGlobalScopeMarker)
                {
                    leftSubstring++;
                    state = 8;
                }
                else if (_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::NonTerminalLocalScopeMarker)
                {
                    leftSubstring++;
                    state = 10;
                }
                else if (_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::TerminalLessThan)
                {
                    leftSubstring++;
                    state = 5;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == 4)
            {
                // Reduce
                // Remove SemiColon
                _symbols.erase(begin(_symbols) + leftSubstring - 1);
                _symbols.insert(begin(_symbols) +  leftSubstring - 1, { .Symbol = LabelParserTreeSymbol::NonTerminalLabel, .Lexeme = string("") });
                break;
            }
            else if (state == 5)
            {
                if (_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::TerminalGBL)
                {
                    scope = LabelScope::Global;
                    leftSubstring++;
                    state = 6;
                }
                else if (_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::TerminalLOC)
                {
                    scope = LabelScope::Local;
                    leftSubstring++;
                    state = 6;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == 6)
            {
                if (_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::TerminalGreaterThan)
                {
                    leftSubstring++;

                    if (scope == LabelScope::Global)
                        state = 7;
                    else
                        state = 9;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == 7)
            {
                // Reduce
                // Remove >
                _symbols.erase(begin(_symbols) + leftSubstring - 1);
                // Remove GBL
                _symbols.erase(begin(_symbols) + leftSubstring - 2);
                // Remove <
                _symbols.erase(begin(_symbols) + leftSubstring - 3);
                _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = LabelParserTreeSymbol::NonTerminalGlobalScopeMarker, .Lexeme = string("") });
                break;
            }
            else if (state == 8)
            {
                if (_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::TerminalSemiColon)
                {
                    // Reduce
                    // Remove :
                    _symbols.erase(begin(_symbols) + leftSubstring - 1);
                    // Remove <GBL>
                    _symbols.erase(begin(_symbols) + leftSubstring - 2);
                    // Remove LabelName
                    _symbols.erase(begin(_symbols) + leftSubstring - 3);
                    _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = LabelParserTreeSymbol::NonTerminalGloballyDefinedLabel, .Lexeme = string("") });
                    break;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == 9)
            {
                
                // Reduce
                // Remove >
                _symbols.erase(begin(_symbols) + leftSubstring - 1);
                // Remove LOC
                _symbols.erase(begin(_symbols) + leftSubstring - 2);
                // Remove <
                _symbols.erase(begin(_symbols) + leftSubstring - 3);
                _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = LabelParserTreeSymbol::NonTerminalLocalScopeMarker, .Lexeme = string("") });
                break;
            }
            else if (state == 10)
            {
                if (_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::TerminalSemiColon)
                {
                    // Reduce
                    // Remove :
                    _symbols.erase(begin(_symbols) + leftSubstring - 1);
                    // Remove <GBL>
                    _symbols.erase(begin(_symbols) + leftSubstring - 2);
                    // Remove LabelName
                    _symbols.erase(begin(_symbols) + leftSubstring - 3);
                    _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = LabelParserTreeSymbol::NonTerminalLocallyDefinedLabel, .Lexeme = string("") });
                    break;
                }
                else
                {
                    Reject(); break;
                }
            }
        }

        cout << "Iteration" << '\n';
        for (auto i : _symbols)
        {
            cout << '\t' << static_cast<size_t>(i.Symbol) << '\n';
        }
    }

    auto intermediateRepresentation = make_shared<LabelIntermediateRepresentation>(identifier, scope, _line, _column);
    return intermediateRepresentation;
}

void LabelSyntacticAnalyzer::ExtractSymbols(vector<Token>::iterator& beginIt, vector<Token>::iterator& endIt)
{
    if (beginIt == endIt)
        return;

    // Only accept operators and identifiers for now (will be expanded with literals soon)
    transform(beginIt, endIt, back_inserter(_symbols), [&](Token x) -> LabelCompoundSymbol
    {
        switch (x.Type)
        {
            case TokenType::Identifier: 
                return {.Symbol = LabelParserTreeSymbol::TerminalIdentifier, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorSEMICOLON: 
                return {.Symbol = LabelParserTreeSymbol::TerminalSemiColon, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorLESSTHAN: 
                return {.Symbol = LabelParserTreeSymbol::TerminalLessThan, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::KeywordGBL: 
                return {.Symbol = LabelParserTreeSymbol::TerminalGBL, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::KeywordLOC: 
                return {.Symbol = LabelParserTreeSymbol::TerminalLOC, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            case TokenType::OperatorGREATERTHAN: 
                return {.Symbol = LabelParserTreeSymbol::TerminalGreaterThan, .Lexeme  = x.Lexeme, .Line = x.Line, .Column = x.Column};
            default:
                return {.Symbol = LabelParserTreeSymbol::TerminalIgnore, .Lexeme  = "", .Line = x.Line, .Column = x.Column};
        }
    });

    auto position = std::find_if(_symbols.begin(), _symbols.end(), [&](LabelCompoundSymbol x) -> auto 
    {
        return x.Symbol == LabelParserTreeSymbol::TerminalIgnore;
    });

    if (position != _symbols.end())
        _symbols.erase(position, _symbols.end());

    if (_symbols.size() != 0)
    {
        _line = _symbols[0].Line;
        _column = _symbols[0].Column;
    }
}

}