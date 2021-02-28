#pragma once

#include <memory>

#include "interfaces/ServerStrategy.h"
#include "interfaces/StrategyParameters.h"
#include "interfaces/Runtime.h"

namespace gbx
{

class DebugServer
{
public:
    DebugServer(std::shared_ptr<gbxcore::interfaces::Runtime>, 
                std::shared_ptr<gbx::interfaces::ServerStrategy>);
    ~DebugServer() = default;

    void Initialize(gbx::interfaces::StrategyParameters);
    void WaitForClient();

private:
    std::shared_ptr<gbxcore::interfaces::Runtime> _gameBoyX;
    std::shared_ptr<gbx::interfaces::ServerStrategy> _strategy;
};

}