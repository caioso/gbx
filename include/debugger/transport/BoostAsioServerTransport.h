#pragma once

#include <boost/asio.hpp>
#include <boost/array.hpp>

#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "BoostAsioTransportBase.h"
#include "ClientJoinedCommand.h"
#include "DebugMessage.h"
#include "DebugMessageNotificationArguments.h"
#include "GBXDebuggerExceptions.h"
#include "MessageID.h"
#include "Observer.h"
#include "ServerTransport.h"

namespace gbxdb::transport
{

class BoostAsioServerTransport : public interfaces::ServerTransport
                               , public BoostAsioTransportBase
{
public:
    BoostAsioServerTransport(std::string, std::string);
    ~BoostAsioServerTransport();

    // Blocking call
    void InitializeProtocol() override;
    void WaitForClient() override;
    void SendMessage(std::shared_ptr<interfaces::DebugMessage>) override;
    void Subscribe(std::weak_ptr<gbxcommons::Observer>) override;
    void Unsubscribe(std::weak_ptr<gbxcommons::Observer>) override;

protected:
    const uint8_t NumberOfStatusPortDefinitionRetries = 0xFF;
    const int ServerStatusPingIntervalInMilliseconds = 400;

    void RunProtocol();
    void ServerStatusLoop();
    void ProtocolLoop();

    void AcceptConnection();
    void NotifyObservers(std::shared_ptr<interfaces::DebugMessage>);
    void InitializeStatusThread();
    void InitializeStatusSocket(); 
    std::shared_ptr<std::array<uint8_t, interfaces::MaxMessageBufferSize>> GenerateInitializationMessage();
    int ChooseStatusPort(int);
    
    std::vector<std::weak_ptr<gbxcommons::Observer>> _observers;
};

}