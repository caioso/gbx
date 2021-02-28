#include "DebugServer.h"

namespace gbx
{

DebugServer::DebugServer(std::shared_ptr<gbxcore::interfaces::Runtime> gbx,
                         std::shared_ptr<gbx::interfaces::ServerStrategy> strategy)
    : _gameBoyX(gbx)
    , _strategy(strategy)
{}

void DebugServer::Initialize(gbx::interfaces::StrategyParameters parameters)
{
    _strategy->Initialize(parameters);
}

void DebugServer::WaitForClient()
{
    _strategy->WaitForClient();
}

}