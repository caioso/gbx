#pragma once

#include "IntermediateRepresentation.h"

namespace gbxasm::intermediate_representation
{

class PackIntermediateRepresentation : public IntermediateRepresentation
{
public:
    PackIntermediateRepresentation();
    ~PackIntermediateRepresentation() = default;
};

}