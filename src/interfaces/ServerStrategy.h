#pragma once

#include "StrategyParameters.h"

namespace gbx::interfaces
{

class ServerStrategy
{
public:
    virtual ~ServerStrategy() = default;

    virtual void Initialize(StrategyParameters) = 0;
    virtual void WaitForClient() = 0;
};

}