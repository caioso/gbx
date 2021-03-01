#pragma once

#include <optional>
#include <variant>

#include "../../interfaces/DebugMessage.h"

namespace gbx::requests
{

enum class DebugServerStatus
{
    Halted,
    Running,
    Error
};

class DebugServerStatusResponse : public interfaces::DebugMessage
{
public:
    DebugServerStatusResponse(DebugServerStatus);
    virtual ~DebugServerStatusResponse() = default;
    
    DebugServerStatus Status();

private:
    DebugServerStatus _status;
};

}