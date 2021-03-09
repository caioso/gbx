#include "ClientJoinedCommand.h"

namespace gbx::protocol
{

ClientJoinedCommand::ClientJoinedCommand()
     : DebugCommand(static_cast<uint16_t>(CommandID::CommandClientJoined))
{}
    
void ClientJoinedCommand::DecodeRequestMessage([[maybe_unused]] std::shared_ptr<interfaces::DebugMessage> message)
{}

std::shared_ptr<interfaces::DebugMessage> ClientJoinedCommand::EncodeRequestMessage()
{
    return {};
}

}