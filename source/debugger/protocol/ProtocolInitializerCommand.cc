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
    auto buffer = make_shared<std::array<uint8_t, MaxMessageBufferSize>>();
    (*buffer)[0] = static_cast<uint16_t>(MessageID::MessageProtocolInitializer) & 0xFF;
    (*buffer)[1] = ((static_cast<uint16_t>(MessageID::MessageProtocolInitializer)) >> 0x08) & 0xFF;

    std::copy(_parameters.begin(), _parameters.end(), (*buffer).begin() + 2);
    return make_shared<DebugMessage>(buffer);
}

void ProtocolInitializerCommand::SetProtocolInitializationParameters(std::array<uint8_t, gbxdb::interfaces::MaxMessageBodySize>& message)
{
    std::copy_n(message.begin(), message.size(), _parameters.begin());
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