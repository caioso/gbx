#pragma once

#include <array>
#include <cstdint>
#include <chrono>
#include <cmath>
#include <memory>
#include <thread>

#include "EngineParameters.h"
#include "GBXExceptions.h"

namespace gbx
{

class ClockSource
{
public:
    ClockSource(uint64_t);
    ~ClockSource() = default;

    void Tick(uint64_t, uint64_t);

    double Period();
    uint64_t Ticks();

private:
    uint64_t _clockPeriodInNanoSeconds;
    uint64_t _ticks;
};

}