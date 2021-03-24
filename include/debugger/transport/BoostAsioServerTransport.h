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
#include "DebugMessage.h"
#include "ServerTransport.h"
#include "DebugMessageNotificationArguments.h"
#include "GBXDebuggerExceptions.h"
#include "Observer.h"

namespace gbxdb::transport
{

class BoostAsioServerTransport : public interfaces::ServerTransport
                               , public BoostAsioTransportBase
{
public:
    BoostAsioServerTransport(std::string, std::string);
    ~BoostAsioServerTransport();

    // Blocking call
    void WaitForClient() override;
    void SendMessage(std::shared_ptr<interfaces::DebugMessage>) override;
    void Subscribe(std::weak_ptr<gbxcommons::Observer>) override;
    void Unsubscribe(std::weak_ptr<gbxcommons::Observer>) override;

protected:
    void RunProtocol();
    void AcceptConnection();
    void ProtocolLoop();
    void NotifyObservers(std::shared_ptr<interfaces::DebugMessage>);
    std::vector<std::weak_ptr<gbxcommons::Observer>> _observers;
};

}