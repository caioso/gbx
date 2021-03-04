#pragma once

#include <memory>

#include "../interfaces/Construction.h"

namespace gbxasm::intermediate_representation
{

enum class ConstructionType
{
    UnknownType = 0,
    Pack   
};

class IntermediateRepresentation
{
public:
    IntermediateRepresentation(ConstructionType);
    virtual ~IntermediateRepresentation() = default;

    ConstructionType Type();
    size_t Column();
    size_t Line();

private:
    ConstructionType _type{};
    size_t _column{};
    size_t _line{};
};

}