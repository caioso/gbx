#include "JoinedServerCommand.h"

using namespace gbxdb;
using namespace gbxdb::interfaces;
using namespace std;

namespace gbxdb::protocol
{

JoinedServerCommand::JoinedServerCommand()
    : DebugCommand(static_cast<uint16_t>(CommandID::CommandJoined))
{}

void JoinedServerCommand::DecodeRequestMessage(shared_ptr<DebugMessage>)
{}

void JoinedServerCommand::DecodeResponseMessage([[maybe_unused]] std::shared_ptr<interfaces::DebugMessage> message)
{}


shared_ptr<DebugMessage> JoinedServerCommand::EncodeCommandMessage()
{
    return {};
}
 
}