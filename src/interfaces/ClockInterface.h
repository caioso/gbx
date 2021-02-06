#pragma once

#include <cstdint>

namespace gbx::interfaces
{

class ClockInterface
{
public:
    virtual uint64_t Ticks() const = 0;
};

}