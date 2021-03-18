#pragma once

#include <memory>

#include "DebugCommand.h"
#include "DebugMessage.h"
#include "ClientCommandID.h"

namespace gbxdb::protocol
{

class JoinedServerClientCommand : public interfaces::DebugCommand
{
public:
    JoinedServerClientCommand();
    ~JoinedServerClientCommand() = default;

    JoinedServerClientCommand(const JoinedServerClientCommand&) = default;
    JoinedServerClientCommand(JoinedServerClientCommand&&) = default;
    JoinedServerClientCommand& operator=(const JoinedServerClientCommand&) = default;
    JoinedServerClientCommand& operator=(JoinedServerClientCommand&&) = default;

    void DecodeRequestMessage(std::shared_ptr<interfaces::DebugMessage>) override;
    std::shared_ptr<interfaces::DebugMessage> EncodeRequestMessage() override;
};

}