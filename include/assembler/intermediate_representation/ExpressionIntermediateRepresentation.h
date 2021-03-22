#pragma once

#include <string>
#include <stack>

#include "ExpressionMember.h"
#include "IntermediateRepresentation.h"
#include "Token.h"

namespace gbxasm::intermediate_representation
{

class ExpressionIntermediateRepresentation : public IntermediateRepresentation
{
public:
    ExpressionIntermediateRepresentation(std::stack<ExpressionMember>, size_t, size_t);
    ~ExpressionIntermediateRepresentation() = default;

    ExpressionIntermediateRepresentation(const ExpressionIntermediateRepresentation&) = default;
    ExpressionIntermediateRepresentation(ExpressionIntermediateRepresentation&&) = default;
    ExpressionIntermediateRepresentation& operator=(const ExpressionIntermediateRepresentation&) = default;
    ExpressionIntermediateRepresentation& operator=(ExpressionIntermediateRepresentation&&) = default;

    std::stack<ExpressionMember> ExpressionStack();

private:
    std::stack<ExpressionMember> _expressionStack{};
};

}