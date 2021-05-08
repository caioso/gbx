#include "FUNCIntermediateRepresentation.h"

using namespace gbxasm::frontend;
using namespace std;
namespace gbxasm::intermediate_representation
{

FUNCIntermediateRepresentation::FUNCIntermediateRepresentation(string identifier, 
                                                               vector<Token> tokens, 
                                                               vector<DeclaredArgument> inputArgs, 
                                                               vector<DeclaredArgument> outputArgs, 
                                                               size_t startLine, 
                                                               size_t startColumn, 
                                                               size_t endLine, 
                                                               size_t endColumn)
    : IntermediateRepresentation(ConstructionType::FUNC, startLine, startColumn, endLine, endColumn)
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