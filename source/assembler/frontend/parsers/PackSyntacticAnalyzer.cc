#include "PackSyntacticAnalyzer.h"

using namespace gbxasm::frontend;
using namespace gbxasm::interfaces;
using namespace gbxasm::intermediate_representation;
using namespace gbxasm::utilities;
using namespace std;

namespace gbxasm::frontend::parsers
{

shared_ptr<gbxasm::intermediate_representation::IntermediateRepresentation> PACKSyntacticAnalyzer::TryToAccept(vector<Token>::iterator& currentToken, vector<Token>::iterator& end)
{
    ExtractSymbols(currentToken, end);

    string identifier;
    vector<DeclaredMember> members;
    DeclaredMember member;

    while (!IsAccepted() && !IsRejected())
    {        
        auto leftSubstring = -1;
        auto state = FSMStates::InitialState;
        
        while(true)
        {
            if (state == FSMStates::InitialState)
            {
                Shift(leftSubstring); state = FSMStates::InitialPackHeaderOrNTPackDetection;
            }
            else if (state == FSMStates::InitialPackHeaderOrNTPackDetection) // Detect 'PACK'
            {
                if (_symbols[leftSubstring].Symbol == PACKParseTreeSymbols::TerminalPack)
                {
                    Shift(leftSubstring); state = FSMStates::PackIdentifierDetection;
                }
                else if (_symbols[leftSubstring].Symbol == PACKParseTreeSymbols::NonTerminalHeader)
                {
                    Shift(leftSubstring); state = FSMStates::InitialMemberDetectionOrFooterOrNTPackDetection;
                }
                else if (_symbols[leftSubstring].Symbol == PACKParseTreeSymbols::NonTerminalPack)
                {
                    Accept(); break;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::PackIdentifierDetection) // Detect 'Identifier'
            {
                if (_symbols[leftSubstring].Symbol == PACKParseTreeSymbols::TerminalIdentifier)
                {
                    Shift(leftSubstring); state = FSMStates::PackBgnDetection;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::PackBgnDetection) // Detect 'Bgn'
            {
                if (_symbols[leftSubstring].Symbol == PACKParseTreeSymbols::TerminalBgn)
                {
                    Shift(leftSubstring); state = FSMStates::PackHeaderDetection;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::PackHeaderDetection) // Detect 'Begin of Members' or 'End'
            {
                if (_symbols[leftSubstring].Symbol == PACKParseTreeSymbols::TerminalType ||
                    _symbols[leftSubstring].Symbol == PACKParseTreeSymbols::TerminalEnd)
                {
                    ReduceHeader(leftSubstring, identifier); break;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::InitialMemberDetectionOrFooterOrNTPackDetection) // Detect Members Type
            {
                if (_symbols[leftSubstring].Symbol == PACKParseTreeSymbols::TerminalType)
                {
                    Shift(leftSubstring); state = FSMStates::MemberIdentifierDetection;
                }
                else if (_symbols[leftSubstring].Symbol == PACKParseTreeSymbols::NonTerminalMember || 
                         _symbols[leftSubstring].Symbol == PACKParseTreeSymbols::NonTerminalMemberList)
                {
                    Shift(leftSubstring); state = FSMStates::MemberListOrFooterOrNTPackDetection;
                }
                else if (_symbols[leftSubstring].Symbol == PACKParseTreeSymbols::TerminalEnd)
                {
                    ReduceEmptyPackFooter(leftSubstring); break;
                }
                else if (_symbols[leftSubstring].Symbol == PACKParseTreeSymbols::NonTerminalFooter)
                {
                    ReduceEmptyPack(leftSubstring); break;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::MemberIdentifierDetection) // Detect Members Identifier
            {
                if (_symbols[leftSubstring].Symbol == PACKParseTreeSymbols::TerminalIdentifier)
                {
                    Shift(leftSubstring); state = FSMStates::NonArrayMemberReduction;
                }
                else if (_symbols[leftSubstring].Symbol == PACKParseTreeSymbols::TerminalOpenBracket)
                {
                    Shift(leftSubstring); state = FSMStates::ArrayDimensionDetection;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::NonArrayMemberReduction) // Detect 'Begin of Members' or 'End' or '['
            {
                if (_symbols[leftSubstring].Symbol == PACKParseTreeSymbols::TerminalType ||
                    _symbols[leftSubstring].Symbol == PACKParseTreeSymbols::TerminalEnd)
                {
                    ReduceMember(leftSubstring, member, members); break;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::ArrayDimensionDetection) // Detect numeric literal
            {
                if (_symbols[leftSubstring].Symbol == PACKParseTreeSymbols::TerminalNumericLiteral)
                {
                    Shift(leftSubstring); state = FSMStates::MemberArrayCloseBracketDetection;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::MemberArrayCloseBracketDetection) // Detect ]
            {
                if (_symbols[leftSubstring].Symbol == PACKParseTreeSymbols::TerminalCloseBracket)
                {
                    Shift(leftSubstring); state = FSMStates::MemberTypeAndDimensionDetection;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::MemberTypeAndDimensionDetection) // Detect 'Begin of Members' or 'End'
            {
                if (_symbols[leftSubstring].Symbol == PACKParseTreeSymbols::TerminalIdentifier)
                {
                    Shift(leftSubstring); state = FSMStates::MemberArrayIdentifierDetected;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::MemberArrayIdentifierDetected)
            {
                if (_symbols[leftSubstring].Symbol == PACKParseTreeSymbols::TerminalType ||
                    _symbols[leftSubstring].Symbol == PACKParseTreeSymbols::TerminalEnd)
                {
                    ReduceArrayMember(leftSubstring, member, members); break;
                }
                else
                {
                    Reject(); break;
                }
            }
            else if (state == FSMStates::MemberListOrFooterOrNTPackDetection) // Detect Member List or Footer Detecton
            {
                if (_symbols[leftSubstring].Symbol == PACKParseTreeSymbols::NonTerminalMember || 
                    _symbols[leftSubstring].Symbol == PACKParseTreeSymbols::NonTerminalMemberList)
                {
                    ReduceMemberList(leftSubstring); break;
                }
                else if (_symbols[leftSubstring].Symbol == PACKParseTreeSymbols::TerminalType)
                {
                    Shift(leftSubstring); state = FSMStates::MemberIdentifierDetection;
                }
                else if (_symbols[leftSubstring].Symbol == PACKParseTreeSymbols::TerminalEnd)
                {
                    ReduceFooter(leftSubstring); break;
                }
                else if (_symbols[leftSubstring].Symbol == PACKParseTreeSymbols::NonTerminalFooter)
                {
                    ReducePack(leftSubstring); break;
                }
                else
                {
                    Reject(); break;
                }
            }
        }

        //cout << "\nOriginal : Size " << _symbols.size() << '\n';
    }

    if (IsAccepted())
        return make_shared<PACKIntermediateRepresentation>(identifier, members, currentToken->Line, currentToken->Column, _endLine, _endColumn);
    else
        return {};
}

void PACKSyntacticAnalyzer::ExtractSymbols(vector<Token>::iterator& currentToken, vector<Token>::iterator& end)
{
    vector<Token> constructionSymbols;

    _symbols.resize(0);
    bool stopCondition = true;
    
    copy_if(currentToken, end, back_inserter(constructionSymbols), [&stopCondition] (Token x) mutable -> auto 
    {
        auto previousCondition = stopCondition;

        if (x.Type == TokenType::KeywordEND)
            stopCondition = false;
        
        return previousCondition;
    });
    
    transform(currentToken, end, back_inserter(_symbols), [&](Token x) -> PACKCompoundSymbol
    {
        switch (x.Type)
        {
            case TokenType::KeywordPACK: 
                return {.Symbol = PACKParseTreeSymbols::TerminalPack, .Lexeme  = x.Lexeme };
            case TokenType::Identifier: 
                return {.Symbol = PACKParseTreeSymbols::TerminalIdentifier, .Lexeme  = x.Lexeme };
            case TokenType::KeywordBGN: 
                return {.Symbol = PACKParseTreeSymbols::TerminalBgn, .Lexeme  = x.Lexeme };
            case TokenType::KeywordBYTE:
            case TokenType::KeywordWORD:
            case TokenType::KeywordDWRD:
            case TokenType::KeywordSTR:
            case TokenType::KeywordCHAR:
            case TokenType::KeywordBOOL:
                 return {.Symbol = PACKParseTreeSymbols::TerminalType, .Lexeme  = x.Lexeme };
            case TokenType::SeparatorOPENBRACKETS: 
                return {.Symbol = PACKParseTreeSymbols::TerminalOpenBracket, .Lexeme  = x.Lexeme };
            case TokenType::SeparatorCLOSEBRACKETS: 
                return {.Symbol = PACKParseTreeSymbols::TerminalCloseBracket, .Lexeme  = x.Lexeme };
            case TokenType::LiteralNumericDECIMAL:
            case TokenType::LiteralNumericHEXADECIMAL:
            case TokenType::LiteralNumericOCTAL:
            case TokenType::LiteralNumericBINARY:
                return {.Symbol = PACKParseTreeSymbols::TerminalNumericLiteral, .Lexeme  = x.Lexeme };
            case TokenType::KeywordEND:
                _endLine = x.Line;
                _endColumn = x.Column + x.Lexeme.size();
                return {.Symbol = PACKParseTreeSymbols::TerminalEnd, .Lexeme  = x.Lexeme };
            default:
            {
                stringstream ss;
                // add a 'translation later'
                ss << "Unexpected '" << static_cast<uint8_t>(x.Type) <<"' found";
                throw SyntacticAnalyzerException(ss.str());
            }
        }
    });
}

inline void PACKSyntacticAnalyzer::Shift(int& top)
{
    top++;
}

inline void PACKSyntacticAnalyzer::ReduceHeader(int top, std::string& identifier)
{
    // Identifier has been found here
    identifier = _symbols[top - 2].Lexeme;

    // Reduce
    // Remove BGN
    _symbols.erase(begin(_symbols) + top - 1);
    // Remove Identifier
    _symbols.erase(begin(_symbols) + top - 2);
    // Remove PACK
    _symbols.erase(begin(_symbols) + top - 3);
    _symbols.insert(begin(_symbols) +  top - 3, { .Symbol = PACKParseTreeSymbols::NonTerminalHeader, .Lexeme = string("") });
}

inline void PACKSyntacticAnalyzer::ReduceEmptyPackFooter(int top)
{
    // Reduce (note that since this is a combination step, leftSubstring is not incremented twice).
    // Remove First
    _symbols.erase(begin(_symbols) + top);
    _symbols.insert(begin(_symbols) +  top, { .Symbol = PACKParseTreeSymbols::NonTerminalFooter, .Lexeme = string("") });
}

inline void PACKSyntacticAnalyzer::ReduceEmptyPack(int top)
{
    // Reduce (note that since this is a combination step, leftSubstring is not incremented twice).
    // Remove First
    _symbols.erase(begin(_symbols) + top);
    // Remove Second
    _symbols.erase(begin(_symbols) + top - 1);
    _symbols.insert(begin(_symbols) +  top - 1, { .Symbol = PACKParseTreeSymbols::NonTerminalPack, .Lexeme = string("") });
}

inline void PACKSyntacticAnalyzer::ReduceMember(int top, DeclaredMember& member, vector<DeclaredMember>& members)
{
    // Reduce
    member.Identifier = _symbols[top - 1].Lexeme;
    member.Type = GetTypeByName(LexemeToDeclaredMemberType::Convert(_symbols[top - 2].Lexeme));
    member.IsArray = false;
    member.ArrayLength = "";
    members.push_back(std::move(member));
    member = {};

    // Remove Identifier
    _symbols.erase(begin(_symbols) + top - 1);
    // Remove Type
    _symbols.erase(begin(_symbols) + top - 2);
    _symbols.insert(begin(_symbols) +  top - 2, { .Symbol = PACKParseTreeSymbols::NonTerminalMember, .Lexeme = string("") });
}

inline void PACKSyntacticAnalyzer::ReduceArrayMember(int top, DeclaredMember& member, vector<DeclaredMember>& members)
{
    // Reduce
    member.Identifier = _symbols[top - 1].Lexeme;
    member.Type = GetTypeByName(LexemeToDeclaredMemberType::Convert(_symbols[top - 5].Lexeme));
    member.IsArray = true;
    member.ArrayLength = _symbols[top - 3].Lexeme;
    members.push_back(std::move(member));
    member = {};

    // Reduce
    // Remove Identifier
    _symbols.erase(begin(_symbols) + top - 1);
    // Remove [
    _symbols.erase(begin(_symbols) + top - 2);
    // Remove Numeric Literal
    _symbols.erase(begin(_symbols) + top - 3);
    // Remove ]
    _symbols.erase(begin(_symbols) + top - 4);
    // Remove Type
    _symbols.erase(begin(_symbols) + top - 5);
    _symbols.insert(begin(_symbols) +  top - 5, { .Symbol = PACKParseTreeSymbols::NonTerminalMember, .Lexeme = string("") });
}

inline void PACKSyntacticAnalyzer::ReduceMemberList(int top)
{
    // Reduce (note that since this is a combination step, leftSubstring is not incremented twice).
    // Remove First
    _symbols.erase(begin(_symbols) + top);
    // Remove Second
    _symbols.erase(begin(_symbols) + top - 1);
    _symbols.insert(begin(_symbols) +  top - 1, { .Symbol = PACKParseTreeSymbols::NonTerminalMemberList, .Lexeme = string("") });
}

inline void PACKSyntacticAnalyzer::ReduceFooter(int top)
{
    // Reduce (note that since this is a combination step, leftSubstring is not incremented twice).
    // Remove First
    _symbols.erase(begin(_symbols) + top);
    _symbols.insert(begin(_symbols) +  top, { .Symbol = PACKParseTreeSymbols::NonTerminalFooter, .Lexeme = string("") });
}

inline void PACKSyntacticAnalyzer::ReducePack(int top)
{
    // Reduce (note that since this is a combination step, leftSubstring is not incremented twice).
    // Remove First
    _symbols.erase(begin(_symbols) + top);
    // Remove Second
    _symbols.erase(begin(_symbols) + top - 1);
    // Remove Third
    _symbols.erase(begin(_symbols) + top - 2);
    _symbols.insert(begin(_symbols) +  top - 2, { .Symbol = PACKParseTreeSymbols::NonTerminalPack, .Lexeme = string("") });
}

}