#include "DebugServer.h"

using namespace std;
using namespace gbx::interfaces;
using namespace gbx::requests;
using namespace gbxcore;
using namespace gbxcore::interfaces;

namespace gbx
{

DebugServer::DebugServer(shared_ptr<Runtime> gbx,
                         shared_ptr<ServerProtocol> protocol)
    : _gameBoyX(gbx)
    , _protocol(protocol)
{}

void DebugServer::Initialize(ServerProtocolParameters parameters)
{
    _protocol->Initialize(parameters);
    _protocol->AddEventListener(shared_from_this());
    _state = DebugServerState::Initialized;
}

void DebugServer::WaitForClient()
{
    if (_state != DebugServerState::Initialized)
        throw DebugServerException("Attempted to perform operaton in an uninitialized debug server");

    _protocol->WaitForClient();
    _state = DebugServerState::WaitingForClient;
}

void DebugServer::Notify(shared_ptr<gbxcommons::NotificationArgs> args)
{
    if (dynamic_pointer_cast<ClientConnectedArgs>(args) != nullptr)
        OnClientConnected();
    else if (dynamic_pointer_cast<MessageReceivedArgs>(args) != nullptr)
        DispatchRequest(dynamic_pointer_cast<MessageReceivedArgs>(args));
    else
        throw DebugServerException("Unknown NotificationArgs type received");
}

void DebugServer::OnClientConnected()
{
    if (_state == DebugServerState::WaitingForClient)
        _state = DebugServerState::Connected;
    else if (_state == DebugServerState::Connected)
        throw DebugServerException("Client already connected");
    else
        throw DebugServerException("Invalid connection request received");
}

void DebugServer::DispatchRequest([[maybe_unused]] shared_ptr<MessageReceivedArgs> message)
{
 /*   if (message->Message->Type() == MessageType::StatusRequest)
    {
        auto response = GenerateStatusResponse();
        _protocol->Send(response);
    }*/
}

shared_ptr<DebugServerStatusResponse> DebugServer::GenerateStatusResponse()
{
    return make_shared<DebugServerStatusResponse>(DebugServerStatus::Halted);
}

// Move this to a separate file
MessageReceivedArgs::MessageReceivedArgs(shared_ptr<DebugMessage> request)
    : Message(request)
{}

}