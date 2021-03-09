#pragma once

#include <memory>

#include "../interfaces/DebugCommand.h"
#include "../interfaces/DebugMessage.h"
#include "CommandID.h"

namespace gbx::protocol
{

class ClientJoinedCommand : public interfaces::DebugCommand
{
public:
    ClientJoinedCommand();
    ~ClientJoinedCommand() = default;

    void DecodeRequestMessage(std::shared_ptr<interfaces::DebugMessage>) override;
    std::shared_ptr<interfaces::DebugMessage> EncodeRequestMessage() override;
};

}