#include "DebugServerStatusResponse.h"

using namespace gbx::interfaces;

namespace gbx::requests
{

DebugServerStatusResponse::DebugServerStatusResponse(DebugServerStatus status)
    : DebugMessage(MessageType::StatusResponse)
    , _status(status)
{}

DebugServerStatus DebugServerStatusResponse::Status()
{
    return _status;
}

}