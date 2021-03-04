#pragma once

#include <boost/asio.hpp>
#include <boost/array.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <thread>

#include "BoostAsioServerProtocolParameters.h"
#include "GBXExceptions.h"
#include "interfaces/DebugMessage.h"
#include "interfaces/ServerProtocol.h"
#include "Observer.h"

namespace gbx
{

class BoostAsioServerProtocol : public interfaces::ServerProtocol
{
public:
    BoostAsioServerProtocol() = default;
    ~BoostAsioServerProtocol() = default;

    virtual void Initialize(std::shared_ptr<interfaces::ServerProtocolParameters>) override;
    virtual void WaitForClient() override;
    virtual void AddEventListener(std::weak_ptr<gbxcommons::Observer<interfaces::RawDebugMessageEventArgs>>) override;
    virtual void Send(std::shared_ptr<interfaces::DebugMessage>) override;

private:
    void RunProtocol();
    void Log(std::string);

    void ProtocolLoop();
    boost::asio::ip::address ConvertIpAddress();
    void AcceptConnection();

    std::vector<std::weak_ptr<gbxcommons::Observer<interfaces::RawDebugMessageEventArgs>>> _observer{};
    std::unique_ptr<std::thread> _thread;

    std::string _ip;
    int _port;
    bool _verbose;
    std::unique_ptr<boost::asio::ip::tcp::socket> _socket;
};

}