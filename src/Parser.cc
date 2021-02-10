#include "Parser.h"

using namespace gbxasm::interfaces;
using namespace gbxasm::constructions;
using namespace gbxasm::parsers;
using namespace std;

namespace gbxasm
{
void Parser::Parse(vector<Token> tokens)
{
    // Evaluate first token en define which parser to use;
    auto currentToken = begin(tokens);
    auto endIterator = end(tokens);

    // Chose appropriate parser
    auto parser = ChooseParser(currentToken);
    auto construction = parser->TryToAccept(currentToken, endIterator);
    
    _acceptedStructures.push_back(construction);
}

vector<AcceptedConstruction> Parser::AcceptedStructures()
{
    return _acceptedStructures;
}

shared_ptr<ConstructionParser> Parser::ChooseParser(__attribute__((unused)) vector<Token>::iterator currentToken)
{
    return make_shared<PackParser>();
}

}