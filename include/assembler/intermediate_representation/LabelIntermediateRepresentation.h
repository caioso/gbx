#pragma once

#include <string>
#include <string_view>

#include "ExpressionMember.h"
#include "IntermediateRepresentation.h"
#include "Token.h"

namespace gbxasm::intermediate_representation
{

enum class LabelScope
{
    Local,
    Global
};

class LabelIntermediateRepresentation : public IntermediateRepresentation
{
public:
    LabelIntermediateRepresentation(std::string, LabelScope, size_t, size_t, size_t, size_t);
    virtual ~LabelIntermediateRepresentation() = default;

    LabelIntermediateRepresentation(const LabelIntermediateRepresentation&) = default;
    LabelIntermediateRepresentation(LabelIntermediateRepresentation&&) = default;
    LabelIntermediateRepresentation& operator=(const LabelIntermediateRepresentation&) = default;
    LabelIntermediateRepresentation& operator=(LabelIntermediateRepresentation&&) = default;

    LabelScope Scope();
    std::string_view Identifier();

private:
    LabelScope _scope;
    std::string _identifier;
};

}