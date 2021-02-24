#include "DebugServer.h"

namespace gbx
{

DebugServer::DebugServer(std::shared_ptr<interfaces::CommunicationChannel> channel)
    : _channel(channel)
{}

}