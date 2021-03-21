#pragma once

#include <memory>

#include "DebugCommand.h"
#include "DebugMessage.h"
#include "CommandID.h"

namespace gbxdb::protocol
{

class JoinedServerCommand : public interfaces::DebugCommand
{
public:
    JoinedServerCommand();
    ~JoinedServerCommand() = default;

    JoinedServerCommand(const JoinedServerCommand&) = default;
    JoinedServerCommand(JoinedServerCommand&&) = default;
    JoinedServerCommand& operator=(const JoinedServerCommand&) = default;
    JoinedServerCommand& operator=(JoinedServerCommand&&) = default;

    void DecodeRequestMessage(std::shared_ptr<interfaces::DebugMessage>) override;
    void DecodeResponseMessage(std::shared_ptr<interfaces::DebugMessage>) override;
    std::shared_ptr<interfaces::DebugMessage> EncodeCommandMessage() override;
};

}