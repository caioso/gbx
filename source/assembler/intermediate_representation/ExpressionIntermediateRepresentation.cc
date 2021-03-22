#include "ExpressionIntermediateRepresentation.h"

using namespace std;

namespace gbxasm::intermediate_representation
{

ExpressionIntermediateRepresentation::ExpressionIntermediateRepresentation(stack<ExpressionMember> expressionStack, size_t line, size_t column)
    : IntermediateRepresentation(ConstructionType::Expression, line, column)
    , _expressionStack(expressionStack)
{}

stack<ExpressionMember> ExpressionIntermediateRepresentation::ExpressionStack()
{
    return _expressionStack;
}

}