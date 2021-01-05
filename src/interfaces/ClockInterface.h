#pragma once

#include <cstdint>

namespace gbx::interfaces
{

class ClockInterface
{
public:
    virtual void Tick(uint64_t, uint64_t) = 0;
};

}