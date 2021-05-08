#pragma once

#include <string>
#include <string_view>

#include "ExpressionMember.h"
#include "IntermediateRepresentation.h"
#include "Token.h"

namespace gbxasm::intermediate_representation
{

class InstructionIntermediateRepresentation : public IntermediateRepresentation
{
public:
    InstructionIntermediateRepresentation(size_t, size_t, size_t, size_t);
    virtual ~InstructionIntermediateRepresentation() = default;

    InstructionIntermediateRepresentation(const InstructionIntermediateRepresentation&) = default;
    InstructionIntermediateRepresentation(InstructionIntermediateRepresentation&&) = default;
    InstructionIntermediateRepresentation& operator=(const InstructionIntermediateRepresentation&) = default;
    InstructionIntermediateRepresentation& operator=(InstructionIntermediateRepresentation&&) = default;
};

}