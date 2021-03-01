#include "DebugServerStatusRequest.h"

using namespace gbx::interfaces;

namespace gbx::requests
{

DebugServerStatusRequest::DebugServerStatusRequest()
    : DebugMessage(MessageType::StatusRequest)
{}

}