#pragma once

#include <memory>

#include "ClockSource.h"
#include "ArithmeticLogicUnitInterface.h"

namespace gbx
{

// Naming following Z80 reference Manual
enum class ControlUnitState
{
    Fetch,
    FetchRealOpcode,
    Decode, 
    Execute,
    WriteBack,
    Acquire,
    Wait,
    Interrupt,
};

class ControlUnit : public std::enable_shared_from_this<ControlUnit>
{
public:
    ControlUnit();
    ~ControlUnit() = default;

    void Initialize(std::shared_ptr<ArithmeticLogicUnitInterface> alu);
    void RunInstructionCycle();

protected:
    std::shared_ptr<ArithmeticLogicUnitInterface> _alu;
    std::unique_ptr<ClockSource> _clock;
    ControlUnitState _state;
};

}