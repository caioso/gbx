#pragma once

#include <cstdlib>

namespace gbx::interfaces
{

enum class MessageType
{
    // Data Requests
    StatusRequest,
    StatusResponse,
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