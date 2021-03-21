#pragma once

#include <algorithm>
#include <array>
#include <memory>

#include "DebugCommand.h"
#include "DebugMessage.h"
#include "GBXDebuggerExceptions.h"
#include "RegisterBankInterface.h"
#include "Runtime.h"
#include "CommandID.h"
#include "MessageID.h"

namespace gbxdb::protocol
{

class RegisterBankSummaryCommand : public interfaces::DebugCommand
{
public:
    RegisterBankSummaryCommand();
    ~RegisterBankSummaryCommand() = default;

    RegisterBankSummaryCommand(const RegisterBankSummaryCommand&) = default;
    RegisterBankSummaryCommand(RegisterBankSummaryCommand&&) = default;
    RegisterBankSummaryCommand& operator=(const RegisterBankSummaryCommand&) = default;
    RegisterBankSummaryCommand& operator=(RegisterBankSummaryCommand&&) = default;

    void SetSummary(std::shared_ptr<gbxcore::interfaces::Runtime>);
    void DecodeRequestMessage(std::shared_ptr<interfaces::DebugMessage>) override;
    void DecodeResponseMessage(std::shared_ptr<interfaces::DebugMessage>) override;
    std::shared_ptr<interfaces::DebugMessage> EncodeCommandMessage() override;

private:
    std::array<uint8_t, 16> _registerValues{};
};

}