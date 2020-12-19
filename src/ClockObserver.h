#pragma once

namespace gbx
{

class ClockObserver
{
public:
    virtual void OnTick() = 0;
    virtual ~ClockObserver() = default;
};

}