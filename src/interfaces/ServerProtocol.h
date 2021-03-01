#pragma once

#include <memory>

#include "DebugMessage.h"
#include "Observer.h"
#include "ServerProtocolParameters.h"

namespace gbx::interfaces
{

class ServerProtocol
{
public:
    virtual ~ServerProtocol() = default;

    virtual void Initialize(ServerProtocolParameters) = 0;
    virtual void WaitForClient() = 0;
    virtual void AddEventListener(std::shared_ptr<gbxcommons::Observer>) = 0;
    virtual void Send(std::shared_ptr<DebugMessage>) = 0;
};

}