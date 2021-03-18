#include "ClientJoinedServerCommand.h"

namespace gbxdb::protocol
{

ClientJoinedServerCommand::ClientJoinedServerCommand()
     : DebugCommand(static_cast<uint16_t>(ServerCommandID::CommandClientJoined))
{}
    
void ClientJoinedServerCommand::DecodeRequestMessage([[maybe_unused]] std::shared_ptr<interfaces::DebugMessage> message)
{}

std::shared_ptr<interfaces::DebugMessage> ClientJoinedServerCommand::EncodeRequestMessage()
{
    return {};
}

}