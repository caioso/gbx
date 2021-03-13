#pragma once

#include <memory>

#include "ArithmeticLogicUnitInterface.h"
#include "MemoryControllerInterface.h"

namespace gbxcore::interfaces
{

class ControlUnitInterface
{
public:
    virtual ~ControlUnitInterface() = default;
    virtual void RunCycle() = 0;
    virtual void Initialize(std::shared_ptr<MemoryControllerInterface>, std::shared_ptr<ArithmeticLogicUnitInterface>) = 0;
};

}