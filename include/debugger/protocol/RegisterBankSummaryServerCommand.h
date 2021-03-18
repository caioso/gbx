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

class RegisterBankSummaryServerCommand : public interfaces::DebugCommand
{
public:
    RegisterBankSummaryServerCommand();
    ~RegisterBankSummaryServerCommand() = default;

    RegisterBankSummaryServerCommand(const RegisterBankSummaryServerCommand&) = default;
    RegisterBankSummaryServerCommand(RegisterBankSummaryServerCommand&&) = default;
    RegisterBankSummaryServerCommand& operator=(const RegisterBankSummaryServerCommand&) = default;
    RegisterBankSummaryServerCommand& operator=(RegisterBankSummaryServerCommand&&) = default;

    void GenerateSummary(std::shared_ptr<gbxcore::interfaces::Runtime>);
    void DecodeRequestMessage(std::shared_ptr<interfaces::DebugMessage>) override;
    std::shared_ptr<interfaces::DebugMessage> EncodeRequestMessage() override;

private:
    std::array<uint8_t, 16> _registerValues;
};

}