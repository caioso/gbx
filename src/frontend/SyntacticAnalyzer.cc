#include "SyntacticAnalyzer.h"

using namespace gbxasm::interfaces;
using namespace gbxasm::constructions;
using namespace gbxasm::parsers;
using namespace std;

namespace gbxasm
{
void SyntacticAnalyzer::Parse(vector<Token> tokens)
{
    // Evaluate first token en define which parser to use;
    auto currentToken = begin(tokens);
    auto endIterator = end(tokens);

    // Chose appropriate parser
    auto parser = ChooseParser(currentToken);
    auto construction = parser->TryToAccept(currentToken, endIterator);
    
    _acceptedStructures.push_back(construction);
}

vector<AcceptedConstruction> SyntacticAnalyzer::AcceptedStructures()
{
    return _acceptedStructures;
}

shared_ptr<ConstructionSyntacticAnalyzer> SyntacticAnalyzer::ChooseParser(__attribute__((unused)) vector<Token>::iterator currentToken)
{
    return make_shared<PackSyntacticAnalyzer>();
}

}