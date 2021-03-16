#pragma once

#include <memory>

#include "DebugCommand.h"
#include "DebugMessage.h"
#include "ServerCommandID.h"

namespace gbxdb::protocol
{

class ClientJoinedCommand : public interfaces::DebugCommand
{
public:
    ClientJoinedCommand();
    ~ClientJoinedCommand() = default;

    ClientJoinedCommand(const ClientJoinedCommand&) = default;
    ClientJoinedCommand(ClientJoinedCommand&&) = default;
    ClientJoinedCommand& operator=(const ClientJoinedCommand&) = default;
    ClientJoinedCommand& operator=(ClientJoinedCommand&&) = default;

    void DecodeRequestMessage(std::shared_ptr<interfaces::DebugMessage>) override;
    std::shared_ptr<interfaces::DebugMessage> EncodeRequestMessage() override;
};

}