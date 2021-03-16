#pragma once

#include <algorithm>
#include <array>
#include <memory>

#include "DebugCommand.h"
#include "DebugMessage.h"
#include "GBXDebuggerExceptions.h"
#include "RegisterBankInterface.h"
#include "Runtime.h"
#include "ServerCommandID.h"
#include "ServerMessageID.h"

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

    void GenerateSummary(std::shared_ptr<gbxcore::interfaces::Runtime>);
    void DecodeRequestMessage(std::shared_ptr<interfaces::DebugMessage>) override;
    std::shared_ptr<interfaces::DebugMessage> EncodeRequestMessage() override;

private:
    std::array<uint8_t, 16> _registerValues;
};

}