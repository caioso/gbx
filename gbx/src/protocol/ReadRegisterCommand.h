#pragma once

#include <memory>

#include "../interfaces/DebugCommand.h"
#include "../interfaces/DebugMessage.h"
#include "../GBXExceptions.h"
#include "interfaces/RegisterBankInterface.h"
#include "CommandID.h"
#include "MessageID.h"

namespace gbx::protocol
{

class ReadRegisterCommand : public interfaces::DebugCommand
{
public:
    ReadRegisterCommand();
    ~ReadRegisterCommand() = default;

    ReadRegisterCommand(const ReadRegisterCommand&) = default;
    ReadRegisterCommand(ReadRegisterCommand&&) = default;
    ReadRegisterCommand& operator=(const ReadRegisterCommand&) = default;
    ReadRegisterCommand& operator=(ReadRegisterCommand&&) = default;

    void SetRegisterValue(uint16_t);
    void DecodeRequestMessage(std::shared_ptr<interfaces::DebugMessage>) override;
    std::shared_ptr<interfaces::DebugMessage> EncodeRequestMessage() override;
    
    gbxcore::interfaces::Register RegisterToRead();

private:
    gbxcore::interfaces::Register _register{};
    uint16_t _registerValue{};
};

}