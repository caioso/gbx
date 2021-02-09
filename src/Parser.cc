#include "Parser.h"

using namespace gbxasm::interfaces;
using namespace gbxasm::constructions;
using namespace std;

namespace gbxasm
{
void Parser::Parse(vector<Token> tokens)
{

}

vector<AcceptedConstruction> Parser::AcceptedStructures()
{
    vector<AcceptedConstruction> result;
    auto construction = make_shared<ConstructionPack>(1, 1);
    AcceptedConstruction acceptedConstruction = 
    { 
        .Type = ConstructionType::Pack,
        .Construction = construction
    };
    
    result.push_back(acceptedConstruction);
    return result;
}

}