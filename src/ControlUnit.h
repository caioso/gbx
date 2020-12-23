#pragma once

#include <memory>

#include "Channel.h"
#include "ArithmeticLogicUnit.h"

namespace gbx
{

// Naming following Z80 reference Manual
enum class ControlUnitState
{
    Fetch, // Cycle M1
};

enum class FetchSubState
{
    FetchT1,    // Fetch PC from memory and increase PC
    FetchT2,
    FetchT3,
    FetchT4     // Decode and possibly execute instruction
};

class ControlUnit
{
public:
    ControlUnit();
    ~ControlUnit() = default;

    void Update();

    // Channels
    std::shared_ptr<Channel<ALUMessage>> ControlUnitALUChannel;

protected:
    void OnALUMessage(ALUMessage message);
    void DecideNextState();
    void UpdateFetch();

    ControlUnitState _state;
    FetchSubState _fetchSubstate;
};

}