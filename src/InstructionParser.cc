#include "InstructionParser.h"

using namespace std;
using namespace gbxasm::interfaces;
using namespace gbxasm::symbols;

namespace gbxasm
{

InstructionParser::InstructionParser(vector<Token>& tokens)
    : _tokens(tokens)
{}

void InstructionParser::ParseTokens()
{
    for (auto token : _tokens)
    {
        auto type = DetectSymbolType(token);
        auto label = make_shared<Label>(token.Token, token.Line, token.Column);
             label->Process();

        _symbolTable.push_back(MakeSymbol(type, static_pointer_cast<BaseSymbol>(label)));
    }
}

SymbolType InstructionParser::DetectSymbolType(Token token)
{
    if (token.Token.find(':') != std::string::npos)
        return SymbolType::Label;
    else 
        return SymbolType::Unknown;
}

}