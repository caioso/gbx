#pragma once

#include <memory>

#include "../interfaces/Construction.h"

namespace gbxasm::frontend
{

enum class ConstructionType
{
    Pack   
};

struct AcceptedConstruction
{
    ConstructionType Type;
    std::shared_ptr<interfaces::Construction> Construction;

};

}