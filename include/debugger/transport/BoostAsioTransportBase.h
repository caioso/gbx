#pragma once

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <chrono>
#include <functional>
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

class BoostAsioTransportBase
{
public:
    BoostAsioTransportBase(std::string, std::string);
    ~BoostAsioTransportBase() = default;

protected:
    const int MainLoopWindDownIntervalInMilliseconds = 100;
    const size_t MaxNumberOfConnections = 1;

    void ListenerLoop(std::function<void(std::shared_ptr<gbxdb::interfaces::DebugMessage>)>);
    void ReceiveMessageBlocking(std::shared_ptr<interfaces::DebugMessage>&, boost::system::error_code&);
    void Terminate();

    boost::asio::ip::address ConvertIpAddress();

    bool _terminated{};
    int _statusPort{};
    
    std::string _ip{};
    std::string _port{};
    std::unique_ptr<std::thread> _mainChannelThread;
    std::unique_ptr<std::thread> _statusChannelThread;
    std::unique_ptr<boost::asio::ip::tcp::socket> _socket;
    std::unique_ptr<boost::asio::ip::tcp::socket> _statusSocket;
    std::recursive_mutex _statusSocketLock;
    std::recursive_mutex _socketLock;
};

}