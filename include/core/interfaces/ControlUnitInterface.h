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
    virtual void Initialize(MemoryControllerInterface*, ArithmeticLogicUnitInterface*) = 0;
};

}