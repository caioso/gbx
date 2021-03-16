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
    auto bufferRef = debugMessageArgs->Message()->Buffer();

    uint16_t messageID = (*bufferRef)[0] | ((*bufferRef)[1] << 0x08);

    // Client messages are handled directly -> *HOWEVER* DebugCommands Still are gonna be generated out of the raw messages.
    if (messageID == ClientMessageID::JoinedServer)
    {
        _isConnected = true;
    }
}

}