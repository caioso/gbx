#pragma once

#include <cstdint>

namespace gbxcore::interfaces
{

class ClockInterface
{
public:
    virtual ~ClockInterface() = default;
    virtual uint64_t Ticks() const = 0;
};

}