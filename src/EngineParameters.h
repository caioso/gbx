#pragma once

#include <cstdint>

namespace gbx
{

class EngineParameters
{
public:
    static const uint64_t GBCFrequency = 8388608;

    static const uint8_t MaxClockObservers = 5;
};

}