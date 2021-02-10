#include "PackParser.h"

using namespace gbxasm::interfaces;
using namespace gbxasm::parsers;
using namespace std;

namespace gbxasm::parsers
{

AcceptedConstruction PackParser::TryToAccept(vector<Token>::iterator& currentToken, vector<Token>::iterator& end)
{
    ExtactSymbols(currentToken, end);

    AcceptedConstruction accepted;

    while (!IsAccepted() && !IsRejected())
    {
        auto state = 0;
        for (auto i = 0llu; i < _symbols.size(); i++)
        {
            if (state == 0)
            {
                if (_symbols[i] == PackSymbols::TerminalPack)
                {
                    _stack.push(_symbols[i]);
                    state++;
                }
                else
                {
                    cout << "Rejected!" << '\n';
                    Reject();
                    break;
                }
            }
            else if (state == 1)
            {
                if (_symbols[i] == PackSymbols::TerminalIdentifier)
                {
                    _stack.push(_symbols[i]);

                    if (i < (_symbols.size() - 1) && _symbols[i + 1] != PackSymbols::TerminalEnd)
                    {
                        cout << "Rejected!" << '\n';
                        Reject();
                        break;
                    }
                    else if (_symbols[i + 1] != PackSymbols::TerminalType && )
                    {

                    }
                }
                else
                {
                    cout << "Rejected!" << '\n';
                    Reject();
                    break;
                }
            }
        }
    }

    return accepted;
}

void PackParser::ExtactSymbols(vector<Token>::iterator& currentToken, vector<Token>::iterator& end)
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
                return PackSymbols::TerminalPack;
            case TokenType::Identifier: 
                return PackSymbols::TerminalIdentifier;
            case TokenType::KeywordBYTE:
            case TokenType::KeywordWORD:
            case TokenType::KeywordDWRD:
            case TokenType::KeywordSTR:
            case TokenType::KeywordCHAR:
            case TokenType::KeywordBOOL:
                 return PackSymbols::TerminalType;
            case TokenType::SeparatorOPENBRACKETS: 
                return PackSymbols::TerminalOpenBracket;
            case TokenType::SeparatorCLOSEBRACKETS: 
                return PackSymbols::TerminalCloseBracket;
            case TokenType::LiteralNumericDECIMAL:
            case TokenType::LiteralNumericHEXADECIMAL:
            case TokenType::LiteralNumericOCTAL:
            case TokenType::LiteralNumericBINARY:
                return PackSymbols::TerminalNumericLiteral;
            case TokenType::KeywordEND:
                return PackSymbols::TerminalEnd;
            default:
            {
                stringstream ss;
                // add a 'translation later'
                ss << "Unexpected '" << static_cast<uint8_t>(x.Type) <<"' found";
                throw ParserException(ss.str());
            }
        }
    });

    for (auto token : _symbols)
    {
        cout << static_cast<size_t>(token) << '\n';
    }
}

}