#include "PackSyntacticAnalyzer.h"

using namespace gbxasm::frontend;
using namespace gbxasm::interfaces;
using namespace gbxasm::intermediate_representation;
using namespace gbxasm::utilities;
using namespace std;

namespace gbxasm::frontend::parsers
{

shared_ptr<gbxasm::intermediate_representation::IntermediateRepresentation> PackSyntacticAnalyzer::TryToAccept(vector<Token>::iterator& currentToken, vector<Token>::iterator& end)
{
    ExtractSymbols(currentToken, end);

    string identifier;
    vector<DeclaredMember> members;

    // current member attributes
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
                if (_symbols[leftSubstring].Symbol == PackParseTreeSymbols::TerminalPack)
                {
                    Shift(leftSubstring); state = FSMStates::PackIdentifierDetection;
                }
                else if (_symbols[leftSubstring].Symbol == PackParseTreeSymbols::NonTerminalHeader)
                {
                    Shift(leftSubstring); state = FSMStates::InitialMemberDetectionOrFooterOrNTPackDetection;
                }
                else if (_symbols[leftSubstring].Symbol == PackParseTreeSymbols::NonTerminalPack)
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
                if (_symbols[leftSubstring].Symbol == PackParseTreeSymbols::TerminalIdentifier)
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
                if (_symbols[leftSubstring].Symbol == PackParseTreeSymbols::TerminalBgn)
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
                if (_symbols[leftSubstring].Symbol == PackParseTreeSymbols::TerminalType ||
                    _symbols[leftSubstring].Symbol == PackParseTreeSymbols::TerminalEnd)
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
                if (_symbols[leftSubstring].Symbol == PackParseTreeSymbols::TerminalType)
                {
                    Shift(leftSubstring); state = FSMStates::MemberIdentifierDetection;
                }
                else if (_symbols[leftSubstring].Symbol == PackParseTreeSymbols::NonTerminalMember || 
                         _symbols[leftSubstring].Symbol == PackParseTreeSymbols::NonTerminalMemberList)
                {
                    Shift(leftSubstring); state = FSMStates::MemberListOrFooterOrNTPackDetection;
                }
                else if (_symbols[leftSubstring].Symbol == PackParseTreeSymbols::TerminalEnd)
                {
                    ReduceEmptyPackFooter(leftSubstring); break;
                }
                else if (_symbols[leftSubstring].Symbol == PackParseTreeSymbols::NonTerminalFooter)
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
                if (_symbols[leftSubstring].Symbol == PackParseTreeSymbols::TerminalIdentifier)
                {
                    Shift(leftSubstring); state = FSMStates::NonArrayMemberReduction;
                }
                else if (_symbols[leftSubstring].Symbol == PackParseTreeSymbols::TerminalOpenBracket)
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
                if (_symbols[leftSubstring].Symbol == PackParseTreeSymbols::TerminalType ||
                    _symbols[leftSubstring].Symbol == PackParseTreeSymbols::TerminalEnd)
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
                if (_symbols[leftSubstring].Symbol == PackParseTreeSymbols::TerminalNumericLiteral)
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
                if (_symbols[leftSubstring].Symbol == PackParseTreeSymbols::TerminalCloseBracket)
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
                if (_symbols[leftSubstring].Symbol == PackParseTreeSymbols::TerminalIdentifier)
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
                if (_symbols[leftSubstring].Symbol == PackParseTreeSymbols::TerminalType ||
                    _symbols[leftSubstring].Symbol == PackParseTreeSymbols::TerminalEnd)
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
                if (_symbols[leftSubstring].Symbol == PackParseTreeSymbols::NonTerminalMember || 
                    _symbols[leftSubstring].Symbol == PackParseTreeSymbols::NonTerminalMemberList)
                {
                    ReduceMemberList(leftSubstring); break;
                }
                else if (_symbols[leftSubstring].Symbol == PackParseTreeSymbols::TerminalType)
                {
                    Shift(leftSubstring); state = FSMStates::MemberIdentifierDetection;
                }
                else if (_symbols[leftSubstring].Symbol == PackParseTreeSymbols::TerminalEnd)
                {
                    ReduceFooter(leftSubstring); break;
                }
                else if (_symbols[leftSubstring].Symbol == PackParseTreeSymbols::NonTerminalFooter)
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
        return make_shared<PackIntermediateRepresentation>(identifier, members, currentToken->Line, currentToken->Column);
    else
        return {};
}

void PackSyntacticAnalyzer::ExtractSymbols(vector<Token>::iterator& currentToken, vector<Token>::iterator& end)
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
    
    transform(currentToken, end, back_inserter(_symbols), [](Token x) -> PackCompoundSymbol
    {
        switch (x.Type)
        {
            case TokenType::KeywordPACK: 
                return {.Symbol = PackParseTreeSymbols::TerminalPack, .Lexeme  = x.Lexeme };
            case TokenType::Identifier: 
                return {.Symbol = PackParseTreeSymbols::TerminalIdentifier, .Lexeme  = x.Lexeme };
            case TokenType::KeywordBGN: 
                return {.Symbol = PackParseTreeSymbols::TerminalBgn, .Lexeme  = x.Lexeme };
            case TokenType::KeywordBYTE:
            case TokenType::KeywordWORD:
            case TokenType::KeywordDWRD:
            case TokenType::KeywordSTR:
            case TokenType::KeywordCHAR:
            case TokenType::KeywordBOOL:
                 return {.Symbol = PackParseTreeSymbols::TerminalType, .Lexeme  = x.Lexeme };
            case TokenType::SeparatorOPENBRACKETS: 
                return {.Symbol = PackParseTreeSymbols::TerminalOpenBracket, .Lexeme  = x.Lexeme };
            case TokenType::SeparatorCLOSEBRACKETS: 
                return {.Symbol = PackParseTreeSymbols::TerminalCloseBracket, .Lexeme  = x.Lexeme };
            case TokenType::LiteralNumericDECIMAL:
            case TokenType::LiteralNumericHEXADECIMAL:
            case TokenType::LiteralNumericOCTAL:
            case TokenType::LiteralNumericBINARY:
                return {.Symbol = PackParseTreeSymbols::TerminalNumericLiteral, .Lexeme  = x.Lexeme };
            case TokenType::KeywordEND:
                return {.Symbol = PackParseTreeSymbols::TerminalEnd, .Lexeme  = x.Lexeme };
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

inline void PackSyntacticAnalyzer::Shift(int& top)
{
    top++;
}

inline void PackSyntacticAnalyzer::ReduceHeader(int top, std::string& identifier)
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
    _symbols.insert(begin(_symbols) +  top - 3, { .Symbol = PackParseTreeSymbols::NonTerminalHeader, .Lexeme = string("") });
}

inline void PackSyntacticAnalyzer::ReduceEmptyPackFooter(int top)
{
    // Reduce (note that since this is a combination step, leftSubstring is not incremented twice).
    // Remove First
    _symbols.erase(begin(_symbols) + top);
    _symbols.insert(begin(_symbols) +  top, { .Symbol = PackParseTreeSymbols::NonTerminalFooter, .Lexeme = string("") });
}

inline void PackSyntacticAnalyzer::ReduceEmptyPack(int top)
{
    // Reduce (note that since this is a combination step, leftSubstring is not incremented twice).
    // Remove First
    _symbols.erase(begin(_symbols) + top);
    // Remove Second
    _symbols.erase(begin(_symbols) + top - 1);
    _symbols.insert(begin(_symbols) +  top - 1, { .Symbol = PackParseTreeSymbols::NonTerminalPack, .Lexeme = string("") });
}

inline void PackSyntacticAnalyzer::ReduceMember(int top, DeclaredMember& member, vector<DeclaredMember>& members)
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
    _symbols.insert(begin(_symbols) +  top - 2, { .Symbol = PackParseTreeSymbols::NonTerminalMember, .Lexeme = string("") });
}

inline void PackSyntacticAnalyzer::ReduceArrayMember(int top, DeclaredMember& member, vector<DeclaredMember>& members)
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
    _symbols.insert(begin(_symbols) +  top - 5, { .Symbol = PackParseTreeSymbols::NonTerminalMember, .Lexeme = string("") });
}

inline void PackSyntacticAnalyzer::ReduceMemberList(int top)
{
    // Reduce (note that since this is a combination step, leftSubstring is not incremented twice).
    // Remove First
    _symbols.erase(begin(_symbols) + top);
    // Remove Second
    _symbols.erase(begin(_symbols) + top - 1);
    _symbols.insert(begin(_symbols) +  top - 1, { .Symbol = PackParseTreeSymbols::NonTerminalMemberList, .Lexeme = string("") });
}

inline void PackSyntacticAnalyzer::ReduceFooter(int top)
{
    // Reduce (note that since this is a combination step, leftSubstring is not incremented twice).
    // Remove First
    _symbols.erase(begin(_symbols) + top);
    _symbols.insert(begin(_symbols) +  top, { .Symbol = PackParseTreeSymbols::NonTerminalFooter, .Lexeme = string("") });
}

inline void PackSyntacticAnalyzer::ReducePack(int top)
{
    // Reduce (note that since this is a combination step, leftSubstring is not incremented twice).
    // Remove First
    _symbols.erase(begin(_symbols) + top);
    // Remove Second
    _symbols.erase(begin(_symbols) + top - 1);
    // Remove Third
    _symbols.erase(begin(_symbols) + top - 2);
    _symbols.insert(begin(_symbols) +  top - 2, { .Symbol = PackParseTreeSymbols::NonTerminalPack, .Lexeme = string("") });
}

}