#include "PackSyntacticAnalyzer.h"

using namespace gbxasm::interfaces;
using namespace gbxasm::parsers;
using namespace std;

namespace gbxasm::parsers
{

AcceptedConstruction PackSyntacticAnalyzer::TryToAccept(vector<Token>::iterator& currentToken, vector<Token>::iterator& end)
{
    ExtactSymbols(currentToken, end);

    AcceptedConstruction accepted;

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
            else if (state == 1) // Detect 'PACK'
            {
                if (_symbols[leftSubstring] == PackParseTreeSymbols::TerminalPack)
                {
                    leftSubstring++;
                    state = 2;
                }
                else if (_symbols[leftSubstring] == PackParseTreeSymbols::NonTerminalHeader)
                {
                    leftSubstring++;
                    state = 4;
                }
                else
                {
                    cout << "Rejected" << '\n';
                    Reject();
                    break;
                }
            }
            else if (state == 2) // Detect 'Identifier'
            {
                if (_symbols[leftSubstring] == PackParseTreeSymbols::TerminalIdentifier)
                {
                    leftSubstring++;
                    state = 3;
                }
                else
                {
                    cout << "Rejected" << '\n';
                    Reject();
                    break;
                }
            }
            else if (state == 3) // Detect 'Begin of Members' or 'End'
            {
                if (_symbols[leftSubstring] == PackParseTreeSymbols::TerminalType ||
                    _symbols[leftSubstring] == PackParseTreeSymbols::TerminalEnd)
                {
                    // Reduce
                    // Remove Identifier
                    _symbols.erase(begin(_symbols) + leftSubstring - 1);
                    // Remove PACK
                    _symbols.erase(begin(_symbols) + leftSubstring - 2);
                    _symbols.insert(begin(_symbols) +  leftSubstring - 2, PackParseTreeSymbols::NonTerminalHeader);
                    break;
                }
                else
                {
                    cout << "Rejected" << '\n';
                    Reject();
                    break;
                }
            }
            else if (state == 4) // Detect Members Type
            {
                if (_symbols[leftSubstring] == PackParseTreeSymbols::TerminalType)
                {
                    leftSubstring++;
                    state = 5;
                }
                else if (_symbols[leftSubstring] == PackParseTreeSymbols::NonTerminalMember || 
                         _symbols[leftSubstring] == PackParseTreeSymbols::NonTerminalMemberList)
                {
                    leftSubstring++;
                    state = 10;
                }
                else if (_symbols[leftSubstring] == PackParseTreeSymbols::TerminalEnd)
                {
                    // Reduce (note that since this is a combination step, leftSubstring is not incremented twice).
                    // Remove First
                    _symbols.erase(begin(_symbols) + leftSubstring);
                    _symbols.insert(begin(_symbols) +  leftSubstring, PackParseTreeSymbols::NonTerminalFooter);
                    break;
                }
                else if (_symbols[leftSubstring] == PackParseTreeSymbols::NonTerminalFooter)
                {
                    Accept();
                    break;
                }
                else
                {
                    cout << "Rejected" << '\n';
                    Reject();
                    break;
                }
            }
            else if (state == 5) // Detect Members Identifier
            {
                if (_symbols[leftSubstring] == PackParseTreeSymbols::TerminalIdentifier)
                {
                    leftSubstring++;
                    state = 6;
                }
                else
                {
                    cout << "Rejected" << '\n';
                    Reject();
                    break;
                }
            }
            else if (state == 6) // Detect 'Begin of Members' or 'End' or '['
            {
                if (_symbols[leftSubstring] == PackParseTreeSymbols::TerminalType ||
                    _symbols[leftSubstring] == PackParseTreeSymbols::TerminalEnd)
                {
                    // Reduce
                    // Remove Identifier
                    _symbols.erase(begin(_symbols) + leftSubstring - 1);
                    // Remove Type
                    _symbols.erase(begin(_symbols) + leftSubstring - 2);
                    _symbols.insert(begin(_symbols) +  leftSubstring - 2, PackParseTreeSymbols::NonTerminalMember);
                    break;
                }
                else if (_symbols[leftSubstring] == PackParseTreeSymbols::TerminalOpenBracket)
                {
                    leftSubstring++;
                    state = 7;
                }
                else
                {
                    cout << "Rejected" << '\n';
                    Reject();
                    break;
                }
            }
            else if (state == 7) // Detect numeric literal
            {
                if (_symbols[leftSubstring] == PackParseTreeSymbols::TerminalNumericLiteral)
                {
                    leftSubstring++;
                    state = 8;
                }
                else
                {
                    cout << "Rejected" << '\n';
                    Reject();
                    break;
                }
            }
            else if (state == 8) // Detect ]
            {
                if (_symbols[leftSubstring] == PackParseTreeSymbols::TerminalCloseBracket)
                {
                    leftSubstring++;
                    state = 9;
                }
                else
                {
                    cout << "Rejected" << '\n';
                    Reject();
                    break;
                }
            }
            else if (state == 9) // Detect 'Begin of Members' or 'End'
            {
                if (_symbols[leftSubstring] == PackParseTreeSymbols::TerminalType ||
                    _symbols[leftSubstring] == PackParseTreeSymbols::TerminalEnd)
                {
                    // Reduce
                    // Remove ]
                    _symbols.erase(begin(_symbols) + leftSubstring - 1);
                    // Remove Numeric Literal
                    _symbols.erase(begin(_symbols) + leftSubstring - 2);
                    // Remove [
                    _symbols.erase(begin(_symbols) + leftSubstring - 3);
                    // Remove Identifier
                    _symbols.erase(begin(_symbols) + leftSubstring - 4);
                    // Remove Type
                    _symbols.erase(begin(_symbols) + leftSubstring - 5);
                    _symbols.insert(begin(_symbols) +  leftSubstring - 5, PackParseTreeSymbols::NonTerminalMember);
                    break;
                }
                else
                {
                    cout << "Rejected" << '\n';
                    Reject();
                    break;
                }
            }
            else if (state == 10) // Detect ]
            {
                if (_symbols[leftSubstring] == PackParseTreeSymbols::NonTerminalMember || 
                    _symbols[leftSubstring] == PackParseTreeSymbols::NonTerminalMemberList)
                {
                    // Reduce (note that since this is a combination step, leftSubstring is not incremented twice).
                    // Remove First
                    _symbols.erase(begin(_symbols) + leftSubstring);
                    // Remove Second
                    _symbols.erase(begin(_symbols) + leftSubstring - 1);
                    _symbols.insert(begin(_symbols) +  leftSubstring - 1, PackParseTreeSymbols::NonTerminalMemberList);
                    break;
                }
                else if (_symbols[leftSubstring] == PackParseTreeSymbols::TerminalType)
                {
                    leftSubstring++;
                    state = 5;
                }
                else if (_symbols[leftSubstring] == PackParseTreeSymbols::TerminalEnd)
                {
                    // Reduce (note that since this is a combination step, leftSubstring is not incremented twice).
                    // Remove First
                    _symbols.erase(begin(_symbols) + leftSubstring);
                    _symbols.insert(begin(_symbols) +  leftSubstring, PackParseTreeSymbols::NonTerminalFooter);
                    break;
                }
                else if (_symbols[leftSubstring] == PackParseTreeSymbols::NonTerminalFooter)
                {
                    Accept();
                    break;
                }
                else
                {
                    cout << "Rejected" << '\n';
                    Reject();
                    break;
                }
            }
        }

        cout << "\nOriginal : Size " << _symbols.size() << '\n';
        for (auto token : _symbols)
        {
            cout << static_cast<size_t>(token) << '\n';
        }
    }

    return accepted;
}

void PackSyntacticAnalyzer::ExtactSymbols(vector<Token>::iterator& currentToken, vector<Token>::iterator& end)
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
    
    transform(currentToken, end, back_inserter(_symbols), [](Token x) -> auto
    {
        switch (x.Type)
        {
            case TokenType::KeywordPACK: 
                return PackParseTreeSymbols::TerminalPack;
            case TokenType::Identifier: 
                return PackParseTreeSymbols::TerminalIdentifier;
            case TokenType::KeywordBYTE:
            case TokenType::KeywordWORD:
            case TokenType::KeywordDWRD:
            case TokenType::KeywordSTR:
            case TokenType::KeywordCHAR:
            case TokenType::KeywordBOOL:
                 return PackParseTreeSymbols::TerminalType;
            case TokenType::SeparatorOPENBRACKETS: 
                return PackParseTreeSymbols::TerminalOpenBracket;
            case TokenType::SeparatorCLOSEBRACKETS: 
                return PackParseTreeSymbols::TerminalCloseBracket;
            case TokenType::LiteralNumericDECIMAL:
            case TokenType::LiteralNumericHEXADECIMAL:
            case TokenType::LiteralNumericOCTAL:
            case TokenType::LiteralNumericBINARY:
                return PackParseTreeSymbols::TerminalNumericLiteral;
            case TokenType::KeywordEND:
                return PackParseTreeSymbols::TerminalEnd;
            default:
            {
                stringstream ss;
                // add a 'translation later'
                ss << "Unexpected '" << static_cast<uint8_t>(x.Type) <<"' found";
                throw SyntacticAnalyzerException(ss.str());
            }
        }
    });

    cout << "Original : Size " << _symbols.size() << '\n';
    for (auto token : _symbols)
    {
        cout << static_cast<size_t>(token) << '\n';
    }
}

}