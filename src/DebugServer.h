#pragma once

#include <iostream>
#include <memory>

#include "interfaces/DebugMessage.h"
#include "interfaces/Runtime.h"
#include "interfaces/ServerProtocol.h"
#include "interfaces/ServerProtocolParameters.h"

#include "requests/client-connected/ClientConnectedRequest.h"
#include "requests/debug-server-status/DebugServerStatusResponse.h"

#include "GBXExceptions.h"
#include "Observer.h"
#include "RequestTranslator.h"

namespace gbx
{

enum class DebugServerState
{
    NotInitialized = 0,
    Initialized,
    WaitingForClient,
    Connected
};

class DebugServer : public gbxcommons::Observer<DebugMessageEventArgs>, public std::enable_shared_from_this<DebugServer>
{
public:
    DebugServer(std::shared_ptr<gbxcore::interfaces::Runtime>, 
                std::shared_ptr<gbx::interfaces::ServerProtocol>);
    ~DebugServer() = default;

    void Initialize(std::shared_ptr<interfaces::ServerProtocolParameters>);
    void WaitForClient();
    void Run();
    
    virtual void Notify(std::shared_ptr<DebugMessageEventArgs>) override;

protected:
    void DispatchRequest(std::shared_ptr<interfaces::DebugMessage>);
    void OnClientConnected();
    void DispatchRequest(interfaces::MessageType);

    std::shared_ptr<requests::DebugServerStatusResponse> GenerateStatusResponse();

    std::shared_ptr<gbxcore::interfaces::Runtime> _gameBoyX;
    std::shared_ptr<gbx::interfaces::ServerProtocol> _protocol;
    std::shared_ptr<RequestTranslator> _translator;
    DebugServerState _state{};
};

}