#include "ExpressionIntermediateRepresentation.h"

using namespace std;

namespace gbxasm::intermediate_representation
{

ExpressionIntermediateRepresentation::ExpressionIntermediateRepresentation(stack<ExpressionMember> expressionStack, size_t startLine, size_t startColumn, size_t endLine, size_t endColumn)
    : IntermediateRepresentation(ConstructionType::Expression, startLine, startColumn, endLine, endColumn)
    , _expressionStack(expressionStack)
{}

stack<ExpressionMember> ExpressionIntermediateRepresentation::ExpressionStack()
{
    return _expressionStack;
}

}