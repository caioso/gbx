#pragma once

namespace gbx
{

class ClockObserver
{
public:
    virtual void Notify() = 0;
    virtual ~ClockObserver() = default;
};

}