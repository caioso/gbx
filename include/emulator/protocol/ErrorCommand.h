#pragma once

#include <memory>

#include "DebugCommand.h"
#include "DebugMessage.h"
#include "CommandID.h"
#include "ErrorID.h"
#include "MessageID.h"

namespace gbx::protocol
{

class ErrorCommand : public interfaces::DebugCommand
{
public:
    ErrorCommand(ErrorID);
    ~ErrorCommand() = default;

    ErrorCommand(const ErrorCommand&) = default;
    ErrorCommand(ErrorCommand&&) = default;
    ErrorCommand& operator=(const ErrorCommand&) = default;
    ErrorCommand& operator=(ErrorCommand&&) = default;

    void DecodeRequestMessage(std::shared_ptr<interfaces::DebugMessage>) override;
    std::shared_ptr<interfaces::DebugMessage> EncodeRequestMessage() override;

private:
    ErrorID _errorId{};
};

}