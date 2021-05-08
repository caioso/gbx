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
        auto state = FSMState::InitialState;

        while(true)
        {
            if (state == FSMState::InitialState)
            {
                Shift(leftSubstring);
                state = FSMState::IdentifyLabelOrAcceptLabel;
            }
            else if (state == FSMState::IdentifyLabelOrAcceptLabel)
            {
                if (_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::TerminalIdentifier)
                {
                    Shift(leftSubstring);
                    state = FSMState::DetectSimpleLabelOrMarkedLabel;
                }
                else if(_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::NonTerminalLabelName)
                {
                    Shift(leftSubstring);
                    state = FSMState::ReduceSimpleLabel;
                }
                else if(IsPartiallyReducedLabel(leftSubstring))
                {
                    Shift(leftSubstring);
                    state = FSMState::ReduceLabel;
                }
                else if (_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::NonTerminalLabel)
                {
                    Accept(); break;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMState::DetectSimpleLabelOrMarkedLabel)
            {
                if (IsSemiColonOrMarkerBegin(leftSubstring))
                {
                    ReduceLabelName(leftSubstring, identifier);
                    break;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMState::ReduceSimpleLabel)
            {
                if (_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::TerminalSemiColon)
                {
                    ReduceSimpleLabel(leftSubstring);
                    break;
                }
                else if (_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::NonTerminalGlobalScopeMarker)
                {
                    Shift(leftSubstring);
                    state = FSMState::ReduceFullGlobalLabel;
                }
                else if (_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::NonTerminalLocalScopeMarker)
                {
                    Shift(leftSubstring);
                    state = FSMState::ReduceFullLocalLabel;
                }
                else if (_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::TerminalLessThan)
                {
                    Shift(leftSubstring);
                    state = FSMState::DetectMakedLabelType;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMState::ReduceLabel)
            {
                ReduceLabel(leftSubstring);
                break;
            }
            else if (state == FSMState::DetectMakedLabelType)
            {
                if (_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::TerminalGBL)
                {
                    scope = LabelScope::Global;
                    Shift(leftSubstring);
                    state = FSMState::DetectMakedEnd;
                }
                else if (_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::TerminalLOC)
                {
                    scope = LabelScope::Local;
                    Shift(leftSubstring);
                    state = FSMState::DetectMakedEnd;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMState::DetectMakedEnd)
            {
                if (_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::TerminalGreaterThan)
                {
                    if (scope == LabelScope::Global)
                        state = FSMState::ReduceGlobalMarker;
                    else
                        state = FSMState::ReduceLocalMarker;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMState::ReduceGlobalMarker)
            {
                ReduceGlobalMarker(leftSubstring);
                break;
            }
            else if (state == FSMState::ReduceFullGlobalLabel)
            {
                if (_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::TerminalSemiColon)
                {
                    ReduceFullGlobalLabel(leftSubstring);
                    break;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMState::ReduceLocalMarker)
            {           
                ReduceLocalMarker(leftSubstring);   
                break;
            }
            else if (state == FSMState::ReduceFullLocalLabel)
            {
                if (_symbols[leftSubstring].Symbol == LabelParserTreeSymbol::TerminalSemiColon)
                {
                    ReduceFullLocalLabel(leftSubstring);
                    break;
                }
                else
                {
                    Reject(); break;
                }
            }
        }
    }

    auto intermediateRepresentation = make_shared<LabelIntermediateRepresentation>(identifier, scope, _line, _column, _endLine, _endColumn);
    return intermediateRepresentation;
}

void LabelSyntacticAnalyzer::ReduceFullLocalLabel(int leftSubstring)
{
    // Reduce
    // Remove :
    _symbols.erase(begin(_symbols) + leftSubstring);
    // Remove <GBL>
    _symbols.erase(begin(_symbols) + leftSubstring - 1);
    // Remove LabelName
    _symbols.erase(begin(_symbols) + leftSubstring - 2);
    _symbols.insert(begin(_symbols) +  leftSubstring - 2, { .Symbol = LabelParserTreeSymbol::NonTerminalLocallyDefinedLabel, .Lexeme = string("") });
}

void LabelSyntacticAnalyzer::ReduceFullGlobalLabel(int leftSubstring)
{
    // Reduce
    // Remove :
    _symbols.erase(begin(_symbols) + leftSubstring);
    // Remove <GBL>
    _symbols.erase(begin(_symbols) + leftSubstring - 1);
    // Remove LabelName
    _symbols.erase(begin(_symbols) + leftSubstring - 2);
    _symbols.insert(begin(_symbols) +  leftSubstring - 2, { .Symbol = LabelParserTreeSymbol::NonTerminalGloballyDefinedLabel, .Lexeme = string("") });
}

void LabelSyntacticAnalyzer::ReduceGlobalMarker(int leftSubstring)
{
    // Reduce
    // Remove >
    _symbols.erase(begin(_symbols) + leftSubstring);
    // Remove GBL
    _symbols.erase(begin(_symbols) + leftSubstring - 1);
    // Remove <
    _symbols.erase(begin(_symbols) + leftSubstring - 2);
    _symbols.insert(begin(_symbols) +  leftSubstring - 2, { .Symbol = LabelParserTreeSymbol::NonTerminalGlobalScopeMarker, .Lexeme = string("") });
}

void LabelSyntacticAnalyzer::ReduceLocalMarker(int leftSubstring)
{
    // Reduce
    // Remove >
    _symbols.erase(begin(_symbols) + leftSubstring);
    // Remove LOC
    _symbols.erase(begin(_symbols) + leftSubstring - 1);
    // Remove <
    _symbols.erase(begin(_symbols) + leftSubstring - 2);
    _symbols.insert(begin(_symbols) +  leftSubstring - 2, { .Symbol = LabelParserTreeSymbol::NonTerminalLocalScopeMarker, .Lexeme = string("") });
}

void LabelSyntacticAnalyzer::ReduceLabel(int leftSubstring)
{
    // Reduce
    // Remove SemiColon
    _symbols.erase(begin(_symbols) + leftSubstring - 1);
    _symbols.insert(begin(_symbols) +  leftSubstring - 1, { .Symbol = LabelParserTreeSymbol::NonTerminalLabel, .Lexeme = string("") });
}

void LabelSyntacticAnalyzer::ReduceLabelName(int leftSubstring, string& identifier)
{
    identifier = (*(begin(_symbols) + leftSubstring - 1)).Lexeme;

    // Reduce
    // Remove SemiColon
    _symbols.erase(begin(_symbols) + leftSubstring - 1);
    _symbols.insert(begin(_symbols) +  leftSubstring - 1, { .Symbol = LabelParserTreeSymbol::NonTerminalLabelName, .Lexeme = string("") });
}

bool LabelSyntacticAnalyzer::IsSemiColonOrMarkerBegin(int leftSubstring)
{
    return _symbols[leftSubstring].Symbol == LabelParserTreeSymbol::TerminalSemiColon || 
           _symbols[leftSubstring].Symbol == LabelParserTreeSymbol::TerminalLessThan;
}

bool LabelSyntacticAnalyzer::IsPartiallyReducedLabel(int leftSubstring)
{
    return _symbols[leftSubstring].Symbol == LabelParserTreeSymbol::NonTerminalSimpleLabel ||
    _symbols[leftSubstring].Symbol == LabelParserTreeSymbol::NonTerminalGloballyDefinedLabel ||
    _symbols[leftSubstring].Symbol == LabelParserTreeSymbol::NonTerminalLocallyDefinedLabel;
}

void LabelSyntacticAnalyzer::ReduceSimpleLabel(int leftSubstring)
{
    // Reduce
    // Remove SemiColon
    _symbols.erase(begin(_symbols) + leftSubstring);
    // Remove Identifier Name
    _symbols.erase(begin(_symbols) + leftSubstring - 1);
    _symbols.insert(begin(_symbols) +  leftSubstring - 1, { .Symbol = LabelParserTreeSymbol::NonTerminalSimpleLabel, .Lexeme = string("") });
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
                _endLine = x.Line;
                _endColumn = x.Column + x.Lexeme.size();
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

inline void LabelSyntacticAnalyzer::Shift(int& top)
{
    top++;
}

}