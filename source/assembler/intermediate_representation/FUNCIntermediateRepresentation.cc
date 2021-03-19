#include "FUNCIntermediateRepresentation.h"

using namespace gbxasm::frontend;
using namespace std;

namespace gbxasm::intermediate_representation
{

FUNCIntermediateRepresentation::FUNCIntermediateRepresentation(string identifier, vector<Token> tokens, size_t line, size_t column)
    : IntermediateRepresentation(ConstructionType::FUNC, line, column)
    , _bodyTokens(tokens)
    , _identifier(identifier)
{}

string_view FUNCIntermediateRepresentation::Identifier()
{
    return _identifier;
}

vector<Token>& FUNCIntermediateRepresentation::BodyTokens()
{
    return _bodyTokens;
}

}