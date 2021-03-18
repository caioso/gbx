#include "JoinedServerClientCommand.h"

using namespace gbxdb;
using namespace gbxdb::interfaces;
using namespace std;

namespace gbxdb::protocol
{

JoinedServerClientCommand::JoinedServerClientCommand()
    : DebugCommand(static_cast<uint16_t>(ClientCommandID::JoinedServer))
{}

void JoinedServerClientCommand::DecodeRequestMessage(shared_ptr<DebugMessage>)
{}

shared_ptr<DebugMessage> JoinedServerClientCommand::EncodeRequestMessage()
{
    return {};
}
 
}