#pragma once

#include <memory>

#include "DebugCommand.h"
#include "DebugMessage.h"
#include "ErrorID.h"
#include "CommandID.h"
#include "MessageID.h"

namespace gbxdb::protocol
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
    void DecodeResponseMessage(std::shared_ptr<interfaces::DebugMessage>) override;
    std::shared_ptr<interfaces::DebugMessage> EncodeCommandMessage() override;

private:
    ErrorID _errorId{};
};

}