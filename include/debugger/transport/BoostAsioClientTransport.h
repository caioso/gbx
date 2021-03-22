#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <string>

#include "ClientTransport.h"
#include "DebugMessage.h"
#include "GBXDebuggerExceptions.h"
#include "Observer.h"

namespace gbxdb::transport
{

class BoostAsioClientTransport : public gbxdb::interfaces::ClientTransport
{
public:
    BoostAsioClientTransport(std::string, std::string);
    ~BoostAsioClientTransport() = default;

    void JoinServer() override;
    void SendMessage(std::shared_ptr<gbxdb::interfaces::DebugMessage>) override;
    void Subscribe(std::weak_ptr<gbxcommons::Observer>) override;
    void Unsubscribe(std::weak_ptr<gbxcommons::Observer>) override;

private:
    boost::asio::ip::address ConvertIpAddress();

    std::string _ip{};
    std::string _port{};
};

}