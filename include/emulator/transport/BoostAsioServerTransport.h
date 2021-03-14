#pragma once

#include <boost/asio.hpp>
#include <boost/array.hpp>

#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "DebugMessage.h"
#include "ServerTransport.h"
#include "DebugMessageNotificationArguments.h"
#include "GBXEmulatorExceptions.h"
#include "Observer.h"

namespace gbx::transport
{

class BoostAsioServerTransport : public interfaces::ServerTransport
{
public:
    BoostAsioServerTransport(std::string, std::string);
    ~BoostAsioServerTransport();

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
    std::mutex _socketLock;
};

}