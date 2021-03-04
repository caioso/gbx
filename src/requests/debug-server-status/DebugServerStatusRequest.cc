#include "DebugServerStatusRequest.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx::requests
{

DebugServerStatusRequest::DebugServerStatusRequest()
    : DebugMessage(MessageType::StatusRequest)
{}

shared_ptr<DebugServerStatusRequest> DebugServerStatusRequest::MakeRequest([[maybe_unused]] array<uint8_t, gbx::interfaces::MaxRawRequestSize> rawBytes)
{
    return make_shared<DebugServerStatusRequest>();
}

}