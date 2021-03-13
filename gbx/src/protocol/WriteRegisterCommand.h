#pragma once

#include <memory>
#include <variant>

#include "../interfaces/DebugCommand.h"
#include "../interfaces/DebugMessage.h"
#include "../GBXExceptions.h"
#include "interfaces/RegisterBankInterface.h"
#include "CommandID.h"
#include "MessageID.h"

namespace gbx::protocol
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
    std::shared_ptr<interfaces::DebugMessage> EncodeRequestMessage() override;
    
    std::variant<uint8_t, uint16_t> RegisterValue();
    gbxcore::interfaces::Register Register();

private:
    bool IsValidRegister(uint8_t);
    void ConvertRegister(uint8_t);

    gbxcore::interfaces::Register _register{};
    uint16_t _registerValue{};
};

}