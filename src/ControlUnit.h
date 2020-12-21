#pragma once

#include <memory>

#include "Channel.h"
#include "ArithmeticLogicUnit.h"

namespace gbx
{

enum class ControlUnitState
{
    Fetch,
};

enum class FetchSubState
{
    FetchT1,
    FetchT2,
    FetchT3,
    FetchT4
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