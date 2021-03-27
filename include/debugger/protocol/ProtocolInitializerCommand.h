#pragma once

#include <iostream>
#include <memory>
#include <variant>

#include "ClientTransport.h"
#include "CommandID.h"
#include "DebugCommand.h"
#include "DebugMessage.h"
#include "MessageID.h"
#include "ServerTransport.h"

namespace gbxdb::protocol
{

class ProtocolInitializerCommand : public interfaces::DebugCommand
{
public:
    ProtocolInitializerCommand();
    ~ProtocolInitializerCommand() = default;

    ProtocolInitializerCommand(const ProtocolInitializerCommand&) = default;
    ProtocolInitializerCommand(ProtocolInitializerCommand&&) = default;
    ProtocolInitializerCommand& operator=(const ProtocolInitializerCommand&) = default;
    ProtocolInitializerCommand& operator=(ProtocolInitializerCommand&&) = default;

    void SetProtocolInitializationParameters(std::array<uint8_t, gbxdb::interfaces::MaxMessageBufferSize>&);
    void InitializeProtocol(std::variant<std::shared_ptr<gbxdb::interfaces::ServerTransport>, std::shared_ptr<gbxdb::interfaces::ClientTransport>>);

    void DecodeRequestMessage(std::shared_ptr<interfaces::DebugMessage>) override;
    void DecodeResponseMessage(std::shared_ptr<interfaces::DebugMessage>) override;
    std::shared_ptr<interfaces::DebugMessage> EncodeCommandMessage() override;

private:
    std::array<uint8_t, gbxdb::interfaces::MaxMessageBufferSize> _parameters{};
};

}