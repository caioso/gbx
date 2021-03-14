#pragma once

#include <algorithm>
#include <array>
#include <memory>

#include "../interfaces/DebugCommand.h"
#include "../interfaces/DebugMessage.h"
#include "../GBXExceptions.h"
#include "interfaces/RegisterBankInterface.h"
#include "interfaces/Runtime.h"
#include "CommandID.h"
#include "MessageID.h"

namespace gbx::protocol
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

    void GenerateSummary(std::shared_ptr<gbxcore::interfaces::Runtime>);
    void DecodeRequestMessage(std::shared_ptr<interfaces::DebugMessage>) override;
    std::shared_ptr<interfaces::DebugMessage> EncodeRequestMessage() override;

private:
    std::array<uint8_t, 16> _registerValues;
};

}