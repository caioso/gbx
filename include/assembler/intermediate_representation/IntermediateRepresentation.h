#pragma once

#include <memory>

#include "Construction.h"

namespace gbxasm::intermediate_representation
{

enum class ConstructionType
{
    UnknownType = 0,
    PACK,
    FUNC,
    Expression,
    Label,
    Instruction
};

class IntermediateRepresentation
{
public:
    IntermediateRepresentation(ConstructionType, size_t, size_t, size_t, size_t);
    virtual ~IntermediateRepresentation() = default;

    IntermediateRepresentation(const IntermediateRepresentation&) = default;
    IntermediateRepresentation(IntermediateRepresentation&&) = default;
    IntermediateRepresentation& operator=(const IntermediateRepresentation&) = default;
    IntermediateRepresentation& operator=(IntermediateRepresentation&&) = default;

    ConstructionType Type();
    size_t StartColumn();
    size_t StartLine();
    size_t EndColumn();
    size_t EndLine();

private:
    ConstructionType _type{};
    size_t _startLine{};
    size_t _startColumn{};
    size_t _endLine{};
    size_t _endColumn{};
};

}