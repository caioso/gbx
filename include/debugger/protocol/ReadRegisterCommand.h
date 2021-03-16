#pragma once

#include <memory>

#include "DebugCommand.h"
#include "DebugMessage.h"
#include "GBXDebuggerExceptions.h"
#include "RegisterBankInterface.h"
#include "ServerCommandID.h"
#include "ServerMessageID.h"

namespace gbxdb::protocol
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