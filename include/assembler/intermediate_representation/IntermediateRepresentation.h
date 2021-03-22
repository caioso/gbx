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
    Expression
};

class IntermediateRepresentation
{
public:
    IntermediateRepresentation(ConstructionType, size_t, size_t);
    virtual ~IntermediateRepresentation() = default;

    IntermediateRepresentation(const IntermediateRepresentation&) = default;
    IntermediateRepresentation(IntermediateRepresentation&&) = default;
    IntermediateRepresentation& operator=(const IntermediateRepresentation&) = default;
    IntermediateRepresentation& operator=(IntermediateRepresentation&&) = default;

    ConstructionType Type();
    size_t Column();
    size_t Line();

private:
    ConstructionType _type{};
    size_t _line{};
    size_t _column{};
};

}