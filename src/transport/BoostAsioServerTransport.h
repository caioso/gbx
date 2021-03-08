#pragma once

#include <boost/asio.hpp>
#include <boost/array.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "../interfaces/DebugMessage.h"
#include "../interfaces/ServerTransport.h"
#include "../runtime/DebugMessageNotificationArguments.h"
#include "../GBXExceptions.h"
#include "Observer.h"

namespace gbx::transport
{

class BoostAsioServerTransport : public interfaces::ServerTransport
{
public:
    BoostAsioServerTransport(std::string, std::string);
    ~BoostAsioServerTransport() = default;

    // Blocking call
    virtual void WaitForClient() override;
    virtual void SendMessage(std::shared_ptr<interfaces::DebugMessage>) override;
    virtual void Subscribe(std::weak_ptr<gbxcommons::Observer>) override;
    virtual void Unsubscribe(std::weak_ptr<gbxcommons::Observer>) override;

protected:
    const size_t MaxNumberOfConnections = 1;

    void RunProtocol();
    void AcceptConnection();
    void ProtocolLoop();
    void NotifyObservers(std::shared_ptr<interfaces::DebugMessage>);
    boost::asio::ip::address ConvertIpAddress();
    
    std::string _ip;
    std::string _port;
    std::unique_ptr<std::thread> _thread;
    std::unique_ptr<boost::asio::ip::tcp::socket> _socket;
    std::vector<std::weak_ptr<gbxcommons::Observer>> _observers;
};

}