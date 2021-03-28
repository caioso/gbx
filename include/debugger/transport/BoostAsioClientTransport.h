#pragma once

#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "BoostAsioTransportBase.h"
#include "ClientTransport.h"
#include "DebugMessage.h"
#include "DebugMessageNotificationArguments.h"
#include "GBXDebuggerExceptions.h"
#include "Observer.h"

namespace gbxdb::transport
{

class BoostAsioClientTransport : public gbxdb::interfaces::ClientTransport
                               , public BoostAsioTransportBase
{
public:
    BoostAsioClientTransport(std::string, std::string);
    ~BoostAsioClientTransport();

    void InitializeProtocol(std::shared_ptr<std::array<uint8_t, gbxdb::interfaces::MaxMessageBufferSize>>) override;
    void JoinServer() override;
    void LeaveServer() override;
    void SendMessage(std::shared_ptr<gbxdb::interfaces::DebugMessage>) override;
    void Subscribe(std::weak_ptr<gbxcommons::Observer>) override;
    void Unsubscribe(std::weak_ptr<gbxcommons::Observer>) override;

private:
    const int SocketRetryIntervalInMilliseconds = 3000;
    const int ClientStatusPingPollingIntervalInMilliseconds = 200;

    void InitializeClientAliveLine();
    void ClientStatusLoop();
    void ExtractStatusPort(std::shared_ptr<std::array<uint8_t, gbxdb::interfaces::MaxMessageBufferSize>>);

    void RunProtocol();
    void TryToJoinServer();
    void ProtocolLoop();
    void NotifyObservers(std::shared_ptr<gbxdb::interfaces::DebugMessage>);
    std::vector<std::weak_ptr<gbxcommons::Observer>> _observers;
};

}