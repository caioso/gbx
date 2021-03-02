#include "SyntacticAnalyzer.h"

using namespace gbxasm::interfaces;
using namespace gbxasm::language;
using namespace gbxasm::frontend;
using namespace gbxasm::frontend::parsers;
using namespace std;

namespace gbxasm::frontend
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

shared_ptr<ConstructionSyntacticAnalyzer> SyntacticAnalyzer::ChooseParser([[maybe_unused]] vector<Token>::iterator currentToken)
{
    return make_shared<PackSyntacticAnalyzer>();
}

}