#pragma once

#include <optional>
#include <variant>

#include "../../interfaces/DebugMessage.h"

namespace gbx::requests
{

class DebugServerStatusRequest : public interfaces::DebugMessage
{
public:
    DebugServerStatusRequest();
    virtual ~DebugServerStatusRequest() = default;
};

}