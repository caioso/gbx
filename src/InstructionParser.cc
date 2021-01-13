#include "InstructionParser.h"

using namespace std;

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
        _symbolTable.push_back(MakeSymbol(type, token.Line));
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