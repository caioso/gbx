#pragma once

#include <iostream>

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

protected:
    void DecideNextState();
    void UpdateFetch();

    ControlUnitState _state;
    FetchSubState _fetchSubstate;
};

}