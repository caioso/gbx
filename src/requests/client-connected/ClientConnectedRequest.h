#pragma once

#include <array>
#include <memory>

#include "../../interfaces/DebugMessage.h"
#include "../../interfaces/ServerProtocol.h"

namespace gbx::requests
{

class ClientConnectedRequest : public interfaces::DebugMessage
{
public:
    ClientConnectedRequest();
    virtual ~ClientConnectedRequest() = default;

    static std::shared_ptr<ClientConnectedRequest> MakeRequest(std::array<uint8_t, gbx::interfaces::MaxRawRequestSize>);
    static std::shared_ptr<interfaces::RawDebugMessageEventArgs> MakeRawRequest();
};

}