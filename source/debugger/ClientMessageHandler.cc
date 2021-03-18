#include "ClientMessageHandler.h"

using namespace gbxcommons;
using namespace gbxdb::interfaces;
using namespace gbxdb::protocol;
using namespace std;

namespace gbxdb
{

ClientMessageHandler::ClientMessageHandler(shared_ptr<ClientTransport> transport)
    : _transport(transport)
{}

void ClientMessageHandler::Initialize()
{
    _transport->Subscribe(shared_from_this());
}

bool ClientMessageHandler::IsConnected()
{
    return _isConnected;
}

void ClientMessageHandler::Notify(shared_ptr<NotificationArguments> args)
{
    auto debugMessageArgs = static_pointer_cast<DebugMessageNotificationArguments>(args);
    ParseMessage(debugMessageArgs->Message());
}

void ClientMessageHandler::ParseMessage(shared_ptr<DebugMessage> message)
{
    uint16_t messageID = (*message->Buffer())[0] | ((*message->Buffer())[1] << 0x08);

    // Client messages are handled directly -> *HOWEVER* DebugCommands Still are gonna be generated out of the raw messages.
    switch (messageID)
    {
        case ClientMessageID::JoinedServer: HandleJoinedServerCommand(message);
             return;           
        default:
            // Send an error command back here!!!!!
            throw MessageHandlerException("Invalid debug message recieved and will be ignored");
    }
}

void ClientMessageHandler::HandleJoinedServerCommand([[maybe_unused]] shared_ptr<DebugMessage> message)
{
    _isConnected = true;
}

}