#pragma once

#include <memory>

#include "DebugCommand.h"
#include "DebugMessage.h"
#include "ServerCommandID.h"

namespace gbxdb::protocol
{

class ClientJoinedServerCommand : public interfaces::DebugCommand
{
public:
    ClientJoinedServerCommand();
    ~ClientJoinedServerCommand() = default;

    ClientJoinedServerCommand(const ClientJoinedServerCommand&) = default;
    ClientJoinedServerCommand(ClientJoinedServerCommand&&) = default;
    ClientJoinedServerCommand& operator=(const ClientJoinedServerCommand&) = default;
    ClientJoinedServerCommand& operator=(ClientJoinedServerCommand&&) = default;

    void DecodeRequestMessage(std::shared_ptr<interfaces::DebugMessage>) override;
    std::shared_ptr<interfaces::DebugMessage> EncodeRequestMessage() override;
};

}