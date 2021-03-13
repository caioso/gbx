#pragma once

#include <cstdint>

namespace gbxcore
{
class EngineParameters
{
public:
    static const uint64_t GBCClockPeriod = 119;
    static const uint8_t MaxClockObservers = 5;
};

}