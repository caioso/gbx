#pragma once

#include <iostream>
#include <memory>

#include "interfaces/DebugMessage.h"
#include "interfaces/Runtime.h"
#include "interfaces/ServerProtocol.h"
#include "interfaces/ServerProtocolParameters.h"

#include "requests/debug-server-status/DebugServerStatusResponse.h"

#include "GBXExceptions.h"
#include "RequestTranslator.h"
#include "Observer.h"

namespace gbx
{

enum class DebugServerState
{
    NotInitialized = 0,
    Initialized,
    WaitingForClient,
    Connected
};

class ClientConnectedArgs : public RequestEventArgs
{
public:
    ~ClientConnectedArgs() = default;
};

class MessageReceivedArgs : public RequestEventArgs
{
public:
    MessageReceivedArgs(std::shared_ptr<interfaces::DebugMessage>);
    ~MessageReceivedArgs() = default;

    std::shared_ptr<interfaces::DebugMessage> Message;
};

class DebugServer : public gbxcommons::Observer<RequestEventArgs>, public std::enable_shared_from_this<DebugServer>
{
public:
    DebugServer(std::shared_ptr<gbxcore::interfaces::Runtime>, 
                std::shared_ptr<gbx::interfaces::ServerProtocol>);
    ~DebugServer() = default;

    void Initialize(std::shared_ptr<interfaces::ServerProtocolParameters>);
    void WaitForClient();
    void DispatchRequest(std::shared_ptr<MessageReceivedArgs>);
    void Run();
    
    virtual void Notify(std::shared_ptr<RequestEventArgs>) override;

protected:
    void OnClientConnected();

    std::shared_ptr<requests::DebugServerStatusResponse> GenerateStatusResponse();

    std::shared_ptr<gbxcore::interfaces::Runtime> _gameBoyX;
    std::shared_ptr<gbx::interfaces::ServerProtocol> _protocol;
    DebugServerState _state{};
};

}