#include "ClientConnectedRequest.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx::requests
{

ClientConnectedRequest::ClientConnectedRequest()
    : DebugMessage(MessageType::ClientConnectedRequest)
{}


std::shared_ptr<ClientConnectedRequest> ClientConnectedRequest::MakeRequest(std::array<uint8_t, gbx::interfaces::MaxRawRequestSize>)
{
    return make_shared<ClientConnectedRequest>();
}

}
