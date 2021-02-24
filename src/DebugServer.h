#pragma once

#include <memory>

#include "interfaces/CommunicationChannel.h"

namespace gbx
{

class DebugServer
{
public:
    DebugServer(std::shared_ptr<interfaces::CommunicationChannel>);
    ~DebugServer() = default;

private:
    std::shared_ptr<interfaces::CommunicationChannel> _channel;
};

}