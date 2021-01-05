#pragma once

#include <array>
#include <cstdint>
#include <chrono>
#include <cmath>
#include <memory>
#include <thread>

#include "EngineParameters.h"
#include "GBXExceptions.h"
#include "interfaces/ClockInterface.h"

namespace gbx
{

class Clock : public interfaces::ClockInterface
{
public:
    Clock(uint64_t);
    virtual ~Clock() = default;

    void Tick(uint64_t, uint64_t);

    double Period();
    uint64_t Ticks();

private:
    uint64_t _clockPeriodInNanoSeconds;
    uint64_t _ticks;
};

}