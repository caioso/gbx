#pragma once

#include <array>
#include <memory>

#include "../../interfaces/DebugMessage.h"
#include "../../interfaces/ServerProtocol.h"

namespace gbx::requests
{

class DebugServerStatusRequest : public interfaces::DebugMessage
{
public:
    DebugServerStatusRequest();
    virtual ~DebugServerStatusRequest() = default;

    static std::shared_ptr<DebugServerStatusRequest> MakeRequest(std::array<uint8_t, gbx::interfaces::MaxRawRequestSize>);
};

}