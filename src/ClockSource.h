#pragma once

#include <array>
#include <cstdint>
#include <chrono>
#include <cmath>
#include <memory>
#include <thread>

#include "ClockObserver.h"
#include "EngineParameters.h"
#include "GBXExceptions.h"

namespace gbx
{

class ClockSource
{
public:
    ClockSource(uint64_t frequencyInHertz);
    ~ClockSource() = default;

    void Subscribe(std::weak_ptr<ClockObserver> observer);
    void Tick();
    uint64_t Ticks();
    double Period();

private:
    void HasBeenRegistered(std::weak_ptr<ClockObserver> observer);

    double _clockPeriod;
    uint64_t _ticks;
    uint32_t _threadSleepDuration;
    uint8_t _observersCounter;
    std::array<std::weak_ptr<ClockObserver>, EngineParameters::MaxClockObservers> _observers;
};

}