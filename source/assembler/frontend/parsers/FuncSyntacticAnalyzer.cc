#include "FUNCSyntacticAnalyzer.h"

using namespace gbxasm;
using namespace gbxasm::frontend;
using namespace gbxasm::intermediate_representation;
using namespace gbxasm::utilities;
using namespace std;

namespace gbxasm::frontend::parsers
{

shared_ptr<gbxasm::intermediate_representation::IntermediateRepresentation> FUNCSyntacticAnalyzer::TryToAccept(vector<Token>::iterator& beginIt, vector<Token>::iterator& endIt)
{
    string identifier{};
    vector<DeclaredArgument> inputArgs;
    vector<DeclaredArgument> outputArgs;

    ExtractSymbols(beginIt, endIt);

    while (!IsAccepted() && !IsRejected())
    {        
        auto leftSubstring = -1; 
        auto state = FSMStates::InitialState;

        while(true)
        {
            if (state == FSMStates::InitialState)
            {
                Shift(leftSubstring); state = FSMStates::DetectFUNC;
            }
            else if (state == FSMStates::DetectFUNC) // Detect 'FUNC'
            {
                if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalFunc)
                {
                    Shift(leftSubstring); state = FSMStates::DetectFUNCIdentifier;
                }
                else if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::NonTerminalHeader) // Detect 'Header'
                {
                    Shift(leftSubstring); state = FSMStates::ArgumentsInitialDetection;
                }
                else if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::NonTerminalFunc) // Detect 'Non-terminal FUNC'
                {
                    Accept(); break;
                }
                else 
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::DetectFUNCIdentifier) // Detect function's identifier
            {
                if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalIdentifier)
                {
                    Shift(leftSubstring); state = FSMStates::ReduceFUNCHeader;
                }
                else 
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::ReduceFUNCHeader) // Try to reduce [BGN found]
            {
                if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalBgn ||
                    _symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalIn ||
                    _symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalOut)
                {
                    ReduceHeader(leftSubstring, identifier); break;
                }
                else 
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::ArgumentsInitialDetection)
            {
                if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalBgn) // 'BGN'
                {
                    Shift(leftSubstring); state = FSMStates::IgnoreBody;
                }
                else if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalIn ||
                         _symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalOut) // Detect Argument
                {
                    Shift(leftSubstring); state = FSMStates::DetectColon;
                }
                else if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::NonTerminalArgument)
                {
                    Shift(leftSubstring); state = FSMStates::MergeArgumentList;
                }
                else if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::NonTerminalArgumentList)
                {
                    Shift(leftSubstring); state = FSMStates::ReduceArgumentList;
                }
                else if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::NonTerminalBody) // 'Body'
                {
                    ReduceBody(leftSubstring); break;
                }
                else 
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::IgnoreBody) // skip the function body
            {
                if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalIgnore)
                {
                    Shift(leftSubstring); state = FSMStates::IgnoreBody;
                }
                else if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalEnd)
                {
                    Shift(leftSubstring); state = FSMStates::ReduceFUNCBody;
                }
                else 
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::ReduceFUNCBody) // Try to reduce [the function body]
            {
                if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalEnd)
                {
                    auto functionBegin = FindFunctionBodyBegin();
                    ReduceBody(functionBegin, leftSubstring); break;
                }
                else 
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::DetectColon)
            {
                if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalSemicolon)
                {
                    Shift(leftSubstring); state = FSMStates::DetectArgumentIdentifier;
                }
                else
                {
                    Reject(); break;
                }
            }        
            else if (state == FSMStates::DetectArgumentIdentifier)
            {
                if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalIdentifier)
                {
                    Shift(leftSubstring); state = FSMStates::ReduceArgumentOrDetectType;
                }
                else
                {
                    Reject(); break;
                }
            }        
            else if (state == FSMStates::ReduceArgumentOrDetectType) // detect Identifier
            {
                if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalAs) // detect AS
                {
                    Shift(leftSubstring); state = FSMStates::DetectArgumentType;
                }
                else if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalBgn ||
                         _symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalIn ||
                         _symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalOut) // detect next argument or body's begin
                {
                    ExtractTypelessArgumentInfo(leftSubstring, inputArgs, outputArgs);
                    ReduceTypelessArgument(leftSubstring);
                    break;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::DetectArgumentType) // Detect Type
            {
                if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalType ||
                    _symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalIdentifier) // Try to reduce
                {
                    Shift(leftSubstring); state = FSMStates::ReduceTypeOrDetectArray;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::ReduceTypeOrDetectArray) // detect '[' or reduce argument
            {
                if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalOpenBracket)
                {
                    Shift(leftSubstring); state = FSMStates::DetectArrayDimensions;
                }
                else if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalBgn ||
                         _symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalIn ||
                         _symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalOut) // detect next argument or body's begin
                {
                    ExtractNonArrayArgumentInfo(leftSubstring, inputArgs, outputArgs);
                    ReduceNonArrayArgument(leftSubstring);
                    break;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::DetectArrayDimensions) // Detect dimensions
            {
                if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalNumericLiteral)
                {
                    Shift(leftSubstring); state = FSMStates::DetectClosedBrackets;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::DetectClosedBrackets) // Detect ']'
            {
                if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalCloseBracket)
                {
                    Shift(leftSubstring); state = FSMStates::ReduceArgumentOrMergeArgumentList;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::ReduceArgumentOrMergeArgumentList) // reduce array argument
            {
                if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalBgn ||
                    _symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalIn ||
                    _symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalOut) // detect next argument or body's begin
                {
                    ExtractArrayArgumentInfo(leftSubstring, inputArgs, outputArgs);
                    ReduceArrayArgument(leftSubstring);
                    break;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::MergeArgumentList) // Detect Argument
            {
                if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalBgn || 
                    _symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalIn ||
                    _symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalOut)
                {
                    ReduceArgumentList(leftSubstring); break;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::ReduceArgumentList) // Detect Argument List
            {
                if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalBgn)
                {
                    Shift(leftSubstring); state = FSMStates::IgnoreBody;
                }
                else if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalIn ||
                         _symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::TerminalOut)
                {
                    Shift(leftSubstring); state = FSMStates::DetectColon;
                }
                else if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::NonTerminalArgument)
                {
                    MergeArgumentsList(leftSubstring); break;
                }
                else if (_symbols[leftSubstring].Symbol == FUNCParseTreeSymbols::NonTerminalBody)
                {
                    ReduceFUNC(leftSubstring);
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

    if (IsRejected())
    {
        _bodyTokens.resize(0);
        return {};
    }

    auto intermediateRepresentation = make_shared<FUNCIntermediateRepresentation>(identifier, _bodyTokens, inputArgs, outputArgs, _line, _column);
    return intermediateRepresentation;
}

void FUNCSyntacticAnalyzer::ExtractSymbols(vector<Token>::iterator& beginIt, vector<Token>::iterator& endIt)
{
    auto functionEnd = beginIt + CountEndWithinFunctionBody(beginIt, endIt);
    auto bgnFound = false;

    _line = (*beginIt).Line;
    _column = (*beginIt).Column;

    // (functionEnd - 1) because only the last END must be converted to 'TerminalEnd'
    // Anything in between becomes 'TerminalIgnore'. Thats why TokenType::KeywordEND is not in the switch block.
    transform(beginIt, (functionEnd - 1), back_inserter(_symbols), [&](Token x) -> FUNCCompoundSymbol
    {
        if (bgnFound && x.Type != TokenType::KeywordEND)
        {
            // Save the body tokens, but the parse tree symbols are top be ignored
            _bodyTokens.push_back(x);
            return {.Symbol = FUNCParseTreeSymbols::TerminalIgnore, .Lexeme  = x.Lexeme };
        }

        switch (x.Type)
        {
            case TokenType::KeywordFUNC: 
                return {.Symbol = FUNCParseTreeSymbols::TerminalFunc, .Lexeme  = x.Lexeme };
            case TokenType::Identifier: 
                return {.Symbol = FUNCParseTreeSymbols::TerminalIdentifier, .Lexeme  = x.Lexeme };
            case TokenType::KeywordIN: 
                return {.Symbol = FUNCParseTreeSymbols::TerminalIn, .Lexeme  = x.Lexeme };
            case TokenType::OperatorSEMICOLON:
                return {.Symbol = FUNCParseTreeSymbols::TerminalSemicolon, .Lexeme  = x.Lexeme };
            case TokenType::KeywordOUT:
                return {.Symbol = FUNCParseTreeSymbols::TerminalOut, .Lexeme  = x.Lexeme };
            case TokenType::KeywordAS:
                return {.Symbol = FUNCParseTreeSymbols::TerminalAs, .Lexeme  = x.Lexeme };
            case TokenType::KeywordBYTE:
            case TokenType::KeywordWORD:
            case TokenType::KeywordDWRD:
            case TokenType::KeywordSTR:
            case TokenType::KeywordCHAR:
            case TokenType::KeywordBOOL:
                 return {.Symbol = FUNCParseTreeSymbols::TerminalType, .Lexeme  = x.Lexeme };
            case TokenType::SeparatorOPENBRACKETS: 
                return {.Symbol = FUNCParseTreeSymbols::TerminalOpenBracket, .Lexeme  = x.Lexeme };
            case TokenType::SeparatorCLOSEBRACKETS: 
                return {.Symbol = FUNCParseTreeSymbols::TerminalCloseBracket, .Lexeme  = x.Lexeme };
            case TokenType::LiteralNumericDECIMAL:
            case TokenType::LiteralNumericHEXADECIMAL:
            case TokenType::LiteralNumericOCTAL:
            case TokenType::LiteralNumericBINARY:
                return {.Symbol = FUNCParseTreeSymbols::TerminalNumericLiteral, .Lexeme  = x.Lexeme };
             case TokenType::KeywordBGN: 
                bgnFound = true; // If bgn has been found, the body of the function started. Ignore everything up to the target end.
                return {.Symbol = FUNCParseTreeSymbols::TerminalBgn, .Lexeme  = x.Lexeme };
            default:
                return {.Symbol = FUNCParseTreeSymbols::TerminalIgnore, .Lexeme  = x.Lexeme };
        }
    });

    if ((*(functionEnd - 1)).Type == TokenType::KeywordEND)
    {
        FUNCCompoundSymbol end = {.Symbol = FUNCParseTreeSymbols::TerminalEnd, .Lexeme  = (*(functionEnd - 1)).Lexeme };
        _symbols.emplace_back(end);
    }
    else
    {
        // Something when wrong.
    }
}

size_t FUNCSyntacticAnalyzer::CountEndWithinFunctionBody(vector<Token>::iterator& beginIt, vector<Token>::iterator& endIt)
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

inline void FUNCSyntacticAnalyzer::Shift(int& top)
{
    top++;
}

void FUNCSyntacticAnalyzer::ReduceHeader(int& leftSubstring, string& identifier)
{
    identifier = _symbols[leftSubstring - 1].Lexeme;

    // Reduce
    // Remove Identifier
    _symbols.erase(begin(_symbols) + leftSubstring - 1);
    // Remove FUNC
    _symbols.erase(begin(_symbols) + leftSubstring - 2);
    _symbols.insert(begin(_symbols) +  leftSubstring - 2, { .Symbol = FUNCParseTreeSymbols::NonTerminalHeader, .Lexeme = string("") });
}

void FUNCSyntacticAnalyzer::ReduceBody(int& leftSubstring)
{
    // Reduce
    // Remove Body
    _symbols.erase(begin(_symbols) + leftSubstring);
    // Remove Header
    _symbols.erase(begin(_symbols) + leftSubstring - 1);
    _symbols.push_back({ .Symbol = FUNCParseTreeSymbols::NonTerminalFunc, .Lexeme = string("") });
}

void FUNCSyntacticAnalyzer::ReduceBody(vector<FUNCCompoundSymbol>::iterator functionBegin, int& leftSubstring)
{
    // Reduce
    // Remove Body
    _symbols.erase(functionBegin, begin(_symbols) + leftSubstring);
    // Add nonterminal body
    _symbols.push_back({ .Symbol = FUNCParseTreeSymbols::NonTerminalBody, .Lexeme = string("") });
}

std::vector<FUNCCompoundSymbol>::iterator FUNCSyntacticAnalyzer::FindFunctionBodyBegin()
{
    auto functionBegin = find_if(_symbols.begin(), _symbols.end(), [&](const FUNCCompoundSymbol& a) -> auto
    {
        return a.Symbol == FUNCParseTreeSymbols::TerminalBgn;
    });
    return functionBegin;
}

void FUNCSyntacticAnalyzer::ExtractTypelessArgumentInfo(int& leftSubstring, std::vector<DeclaredArgument>& inputArgs, std::vector<DeclaredArgument>& outputArgs)
{
    DeclaredArgument arg;
    arg.Identifier = _symbols[leftSubstring - 1].Lexeme;
    arg.Direction = _symbols[leftSubstring - 3].Lexeme.compare(Lexemes::KeywordIN) == 0? ArgumentDirection::Input : ArgumentDirection::Output;
    arg.Type = TypeUnknown;
    arg.ArrayLength = "0";
    arg.IsArray = false;

    if (arg.Direction == ArgumentDirection::Input)
        inputArgs.push_back(arg);
    else
        outputArgs.push_back(arg);
}

void FUNCSyntacticAnalyzer::ReduceTypelessArgument(int& leftSubstring)
{
    // Reduce
    // Remove identifier
    _symbols.erase(begin(_symbols) + leftSubstring - 1);
    // Remove Colon
    _symbols.erase(begin(_symbols) + leftSubstring - 2);
    // Remove IN/Out
    _symbols.erase(begin(_symbols) + leftSubstring - 3);
    // Add nonterminal argument
    _symbols.insert(begin(_symbols) +  leftSubstring - 3, { .Symbol = FUNCParseTreeSymbols::NonTerminalArgument, .Lexeme = string("") });
}

void FUNCSyntacticAnalyzer::ExtractNonArrayArgumentInfo(int& leftSubstring, std::vector<DeclaredArgument>& inputArgs, std::vector<DeclaredArgument>& outputArgs)
{
    DeclaredArgument arg;
    arg.Identifier = _symbols[leftSubstring - 3].Lexeme;
    arg.Direction = _symbols[leftSubstring - 5].Lexeme.compare(Lexemes::KeywordIN) == 0? ArgumentDirection::Input : ArgumentDirection::Output;
    
    if (_symbols[leftSubstring - 1].Symbol == FUNCParseTreeSymbols::TerminalIdentifier)
        arg.Type = {.Name = TypeName::Custom, .LexicalTypeName = _symbols[leftSubstring - 1].Lexeme, .Size = numeric_limits<size_t>::max()};
    else
        arg.Type = GetTypeByName(LexemeToDeclaredMemberType::Convert(_symbols[leftSubstring - 1].Lexeme));

    arg.ArrayLength = "0";
    arg.IsArray = false;

    if (arg.Direction == ArgumentDirection::Input)
        inputArgs.push_back(arg);
    else
        outputArgs.push_back(arg);
}

void FUNCSyntacticAnalyzer::ReduceNonArrayArgument(int& leftSubstring)
{
    // Reduce
    // Remove Type
    _symbols.erase(begin(_symbols) + leftSubstring - 1);
    // Remove AS
    _symbols.erase(begin(_symbols) + leftSubstring - 2);
    // Remove identifier
    _symbols.erase(begin(_symbols) + leftSubstring - 3);
    // Remove Colon
    _symbols.erase(begin(_symbols) + leftSubstring - 4);
    // Remove IN/Out
    _symbols.erase(begin(_symbols) + leftSubstring - 5);
    // Add nonterminal argument
    _symbols.insert(begin(_symbols) +  leftSubstring - 5, { .Symbol = FUNCParseTreeSymbols::NonTerminalArgument, .Lexeme = string("") });
}

void FUNCSyntacticAnalyzer::ExtractArrayArgumentInfo(int& leftSubstring, std::vector<DeclaredArgument>& inputArgs, std::vector<DeclaredArgument>& outputArgs)
{
    DeclaredArgument arg;
    arg.Identifier = _symbols[leftSubstring - 6].Lexeme;
    arg.Direction = _symbols[leftSubstring - 8].Lexeme.compare(Lexemes::KeywordIN) == 0? ArgumentDirection::Input : ArgumentDirection::Output;
    
    if (_symbols[leftSubstring - 4].Symbol == FUNCParseTreeSymbols::TerminalIdentifier)
        arg.Type = {.Name = TypeName::Custom, .LexicalTypeName = _symbols[leftSubstring - 4].Lexeme, .Size = numeric_limits<size_t>::max()};
    else
        arg.Type = GetTypeByName(LexemeToDeclaredMemberType::Convert(_symbols[leftSubstring - 4].Lexeme));

    arg.ArrayLength = _symbols[leftSubstring - 2].Lexeme;
    arg.IsArray = true;

    if (arg.Direction == ArgumentDirection::Input)
        inputArgs.push_back(arg);
    else
        outputArgs.push_back(arg);
}

void FUNCSyntacticAnalyzer::ReduceArrayArgument(int& leftSubstring)
{
    // Reduce
    // Remove ]
    _symbols.erase(begin(_symbols) + leftSubstring - 1);
    // Remove Numeric Literal
    _symbols.erase(begin(_symbols) + leftSubstring - 2);
    // Remove [
    _symbols.erase(begin(_symbols) + leftSubstring - 3);
    // Remove Type
    _symbols.erase(begin(_symbols) + leftSubstring - 4);
    // Remove AS
    _symbols.erase(begin(_symbols) + leftSubstring - 5);
    // Remove identifier
    _symbols.erase(begin(_symbols) + leftSubstring - 6);
    // Remove Colon
    _symbols.erase(begin(_symbols) + leftSubstring - 7);
    // Remove IN/Out
    _symbols.erase(begin(_symbols) + leftSubstring - 8);
    // Add nonterminal argument
    _symbols.insert(begin(_symbols) +  leftSubstring - 8, { .Symbol = FUNCParseTreeSymbols::NonTerminalArgument, .Lexeme = string("") });
}

void FUNCSyntacticAnalyzer::ReduceArgumentList(int& leftSubstring)
{
    // Reduce
    // Remove Argument
    _symbols.erase(begin(_symbols) + leftSubstring - 1);
    // Add Argument List
    _symbols.insert(begin(_symbols) +  leftSubstring - 1, { .Symbol = FUNCParseTreeSymbols::NonTerminalArgumentList, .Lexeme = string("") });
}

void FUNCSyntacticAnalyzer::MergeArgumentsList(int& leftSubstring)
{
    // Reduce
    // Remove Argument (only argument list remain)
    _symbols.erase(begin(_symbols) + leftSubstring);
}

void FUNCSyntacticAnalyzer::ReduceFUNC(int& leftSubstring)
{
    // Reduce
    // Remove Body
    _symbols.erase(begin(_symbols) + leftSubstring);
    // Remove ArguementList
    _symbols.erase(begin(_symbols) + leftSubstring - 1);
    // Remove Header
    _symbols.erase(begin(_symbols) + leftSubstring - 2);
    _symbols.push_back({ .Symbol = FUNCParseTreeSymbols::NonTerminalFunc, .Lexeme = string("") });
}

}