#pragma once

#include <cstdint>

namespace gbxcore::interfaces
{

class ClockInterface
{
public:
    virtual uint64_t Ticks() const = 0;
};

}