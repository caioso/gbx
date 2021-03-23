#pragma once

#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "ClientTransport.h"
#include "DebugMessage.h"
#include "DebugMessageNotificationArguments.h"
#include "GBXDebuggerExceptions.h"
#include "Observer.h"

namespace gbxdb::transport
{

class BoostAsioClientTransport : public gbxdb::interfaces::ClientTransport
{
public:
    BoostAsioClientTransport(std::string, std::string);
    ~BoostAsioClientTransport();

    void JoinServer() override;
    void LeaveServer() override;
    void SendMessage(std::shared_ptr<gbxdb::interfaces::DebugMessage>) override;
    void Subscribe(std::weak_ptr<gbxcommons::Observer>) override;
    void Unsubscribe(std::weak_ptr<gbxcommons::Observer>) override;

private:
    void RunProtocol();
    void TryToJoinServer();
    void ProtocolLoop();

    void NotifyObservers(std::shared_ptr<gbxdb::interfaces::DebugMessage>);

    boost::asio::ip::address ConvertIpAddress();
    std::unique_ptr<std::thread> _thread;
    std::unique_ptr<boost::asio::ip::tcp::socket> _socket;
    std::vector<std::weak_ptr<gbxcommons::Observer>> _observers;
    std::mutex _socketLock;

    std::string _ip{};
    std::string _port{};
    bool _terminated{};
};

}