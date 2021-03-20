#include "FUNCIntermediateRepresentation.h"

using namespace gbxasm::frontend;
using namespace std;

namespace gbxasm::intermediate_representation
{

FUNCIntermediateRepresentation::FUNCIntermediateRepresentation(string identifier, 
                                                               vector<Token> tokens, 
                                                               vector<DeclaredArgument> inputArgs, 
                                                               vector<DeclaredArgument> outputArgs, 
                                                               size_t line, 
                                                               size_t column)
    : IntermediateRepresentation(ConstructionType::FUNC, line, column)
    , _inputArguments(inputArgs)
    , _outputArguments(outputArgs)
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

vector<DeclaredArgument>& FUNCIntermediateRepresentation::InputArguments()
{
    return _inputArguments;
}

vector<DeclaredArgument>& FUNCIntermediateRepresentation::OutputArguments()
{
    return _outputArguments;
}

}