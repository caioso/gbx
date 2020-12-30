#pragma once

#include <memory>

#include "Channel.h"
#include "ClockSource.h"
#include "ArithmeticLogicUnit.h"

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

    void Initialize();
    void RunInstructionCycle();

    // Channels
    std::shared_ptr<Channel<ALUMessage>> ControlUnitALUChannel;

protected:
    inline void OnALUMessage(ALUMessage);
    inline void DecideNextState();
    inline void Fetch();
    inline void FetchRealOpcode();
    inline void Decode();
    inline void Execute();
    inline void WriteBack();
    inline void Acquire();
    inline void Wait();

    std::unique_ptr<ClockSource> _clock;
    ControlUnitState _state;
};

}