#include "ClientJoinedCommand.h"

using namespace gbxdb::interfaces;
using namespace std;

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
    auto debugMessage = make_shared<gbxdb::interfaces::DebugMessage>(make_shared<array<uint8_t, MaxMessageBufferSize>>());
    (*debugMessage->Buffer())[0] = MessageID::MessageJoined & 0xFF;
    (*debugMessage->Buffer())[1] = (MessageID::MessageJoined >> 0x08) & 0xFF;
    return debugMessage;
}

}