#include "ProtocolInitializerCommand.h"

using namespace std;
using namespace gbxdb;
using namespace gbxdb::interfaces;

namespace gbxdb::protocol
{

ProtocolInitializerCommand::ProtocolInitializerCommand()
     : DebugCommand(static_cast<uint16_t>(CommandID::CommandProtocolInitializer))
{}
    
void ProtocolInitializerCommand::DecodeRequestMessage([[maybe_unused]] std::shared_ptr<interfaces::DebugMessage> message)
{}

void ProtocolInitializerCommand::DecodeResponseMessage([[maybe_unused]] std::shared_ptr<interfaces::DebugMessage> message)
{}

std::shared_ptr<interfaces::DebugMessage> ProtocolInitializerCommand::EncodeCommandMessage()
{
    cout << "Encoding Protocol Initializer Message" << '\n';
    auto buffer = make_shared<std::array<uint8_t, MaxMessageBufferSize>>();
    (*buffer)[0] = static_cast<uint16_t>(MessageID::MessageProtocolInitializer) & 0xFF;
    (*buffer)[1] = ((static_cast<uint16_t>(MessageID::MessageProtocolInitializer)) >> 0x08) & 0xFF;

    std::copy(_parameters.begin(), _parameters.end() - 2, (*buffer).begin() + 2);

    size_t port = (*buffer)[2] | (*buffer)[3] << 0x08 | (*buffer)[4] << 0x10 << (*buffer)[5] << 0x18;
    cout << "Encoded port " << hex << port << '\n';

    return make_shared<DebugMessage>(buffer);
}

void ProtocolInitializerCommand::SetProtocolInitializationParameters(std::array<uint8_t, gbxdb::interfaces::MaxMessageBufferSize>& message)
{
    std::copy(message.begin(), message.end(), _parameters.begin());
}

void ProtocolInitializerCommand::InitializeProtocol(variant<shared_ptr<ServerTransport>, shared_ptr<ClientTransport>> target)
{
    if (holds_alternative<shared_ptr<ServerTransport>>(target))
    {
        auto serverTransport = get<shared_ptr<ServerTransport>>(target);
        serverTransport->InitializeProtocol();
    }
}

}