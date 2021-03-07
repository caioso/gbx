#pragma once

#include <memory>

#include "../interfaces/DebugCommand.h"
#include "../interfaces/DebugMessage.h"
#include "../GBXExceptions.h"
#include "interfaces/RegisterBankInterface.h"
#include "CommandID.h"

namespace gbx::protocol
{

class ReadRegisterCommand : public interfaces::DebugCommand
{
public:
    ReadRegisterCommand();
    ~ReadRegisterCommand() = default;

    void SetRegisterValue(uint16_t);
    virtual void DecodeRequestMessage(std::shared_ptr<interfaces::DebugMessage>) override;
    virtual std::shared_ptr<interfaces::DebugMessage> EncodeRequestMessage() override;
    
    gbxcore::interfaces::Register RegisterToRead();

private:
    gbxcore::interfaces::Register _register{};
    uint16_t _registerValue{};
};

}