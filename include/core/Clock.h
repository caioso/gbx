#pragma once

#include <array>
#include <cstdint>
#include <chrono>
#include <cmath>
#include <memory>
#include <thread>

#include "EngineParameters.h"
#include "GBXCoreExceptions.h"
#include "interfaces/ClockInterface.h"

namespace gbxcore
{

class Clock : public interfaces::ClockInterface
{
public:
    explicit Clock(uint64_t);
    virtual ~Clock() = default;
    
    Clock(const Clock&) = default; 
    Clock(Clock&&) = default;
    Clock& operator=(const Clock&) = default;
    Clock& operator=(Clock&&) = default;

    void Tick(uint64_t, uint64_t);

    double Period() const;
    virtual uint64_t Ticks() const override;

private:
    uint64_t _clockPeriodInNanoSeconds;
    uint64_t _ticks;
};

}