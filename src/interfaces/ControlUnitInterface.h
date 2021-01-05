#pragma once

#include <memory>

#include "ArithmeticLogicUnitInterface.h"
#include "MemoryControllerInterface.h"

namespace gbx::interfaces
{

class ControlUnitInterface
{
public:
    virtual void RunCycle() = 0;
    virtual void Initialize(std::shared_ptr<interfaces::MemoryControllerInterface>, std::shared_ptr<ArithmeticLogicUnitInterface>) = 0;
};

}