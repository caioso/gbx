#pragma once

#include <memory>
#include <variant>

#include "DebugCommand.h"
#include "DebugMessage.h"
#include "GBXDebuggerExceptions.h"
#include "RegisterBankInterface.h"
#include "CommandID.h"
#include "MessageID.h"

namespace gbxdb::protocol
{

class WriteRegisterCommand : public interfaces::DebugCommand
{
public:
    WriteRegisterCommand();
    ~WriteRegisterCommand() = default;

    WriteRegisterCommand(const WriteRegisterCommand&) = default;
    WriteRegisterCommand(WriteRegisterCommand&&) = default;
    WriteRegisterCommand& operator=(const WriteRegisterCommand&) = default;
    WriteRegisterCommand& operator=(WriteRegisterCommand&&) = default;

    void DecodeRequestMessage(std::shared_ptr<interfaces::DebugMessage>) override;
    void DecodeResponseMessage(std::shared_ptr<interfaces::DebugMessage>) override;
    std::shared_ptr<interfaces::DebugMessage> EncodeCommandMessage() override;
    
    std::variant<uint8_t, uint16_t> RegisterValue();
    gbxcore::interfaces::Register Register();

private:
    bool IsValidRegister(uint8_t);
    void ConvertRegister(uint8_t);

    gbxcore::interfaces::Register _register{};
    uint16_t _registerValue{};
};

}