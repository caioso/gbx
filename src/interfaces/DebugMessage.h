#pragma once

#include <cstdlib>

namespace gbx::interfaces
{

enum class MessageType
{
    // Client Connected Message
    ClientConnectedRequest,
    // Server Status Messages
    StatusRequest,
    StatusResponse,
    // Register Status Messages
    RegisterRequest,
    RegisterResponse,
    UnknownMessage
};

class DebugMessage
{
public:
    DebugMessage(MessageType);
    virtual ~DebugMessage() = default;

    MessageType Type();

protected:
    MessageType _requestType;
};

}