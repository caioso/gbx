#include "ClientJoinedCommand.h"

namespace gbxdb::protocol
{

ClientJoinedCommand::ClientJoinedCommand()
     : DebugCommand(static_cast<uint16_t>(CommandID::CommandJoined))
{}
    
void ClientJoinedCommand::DecodeRequestMessage([[maybe_unused]] std::shared_ptr<interfaces::DebugMessage> message)
{}

void ClientJoinedCommand::DecodeResponseMessage([[maybe_unused]] std::shared_ptr<interfaces::DebugMessage> message)
{}

std::shared_ptr<interfaces::DebugMessage> ClientJoinedCommand::EncodeCommandMessage()
{
    return {};
}

}