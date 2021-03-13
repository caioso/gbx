#include "WriteRegisterCommand.h"

using namespace gbx::interfaces;
using namespace gbxcore::interfaces;
using namespace std;

namespace gbx::protocol
{

WriteRegisterCommand::WriteRegisterCommand()
    : DebugCommand(CommandID::CommandWriteRegister)
{}

void WriteRegisterCommand::DecodeRequestMessage(shared_ptr<DebugMessage> message)
{
    auto targetRegister = (*message->Buffer())[2];

    if (!IsValidRegister(targetRegister))
        throw MessageHandlerException("Invalid target register found when parsing 'WriteRegister' command");
    
    ConvertRegister(targetRegister);
    _registerValue = (*message->Buffer())[3] | ((*message->Buffer())[4] << 0x08);
}

void WriteRegisterCommand::ConvertRegister(uint8_t targetRegister)
{
    auto validRegisters = {Register::B, Register::C, Register::D, Register::E, Register::H, Register::L,
                        Register::A, Register::F, Register::IR, Register::PIR, Register::R, Register::PC,
                        Register::SP, Register::IX, Register::IY,  Register::HL, Register::BC, Register::DE, Register::AF};

    for (auto reg : validRegisters)
        if (static_cast<uint8_t>(reg) == targetRegister)
        {
            _register = reg;
        }
}

bool WriteRegisterCommand::IsValidRegister(uint8_t targetRegister)
{
    auto validRegisters = {Register::B, Register::C, Register::D, Register::E, Register::H, Register::L,
                        Register::A, Register::F, Register::IR, Register::PIR, Register::R, Register::PC,
                        Register::SP, Register::IX, Register::IY,  Register::HL, Register::BC, Register::DE, Register::AF};

    for (auto reg : validRegisters)
        if (static_cast<uint8_t>(reg) == targetRegister)
        {
            return true;
        }

    return false;
}

std::variant<uint8_t, uint16_t> WriteRegisterCommand::RegisterValue()
{
    if (!gbxcore::interfaces::RegisterBankInterface::IsPair(_register))
        return static_cast<uint8_t>(_registerValue);
    else
        return static_cast<uint16_t>(_registerValue);
}

gbxcore::interfaces::Register WriteRegisterCommand::Register()
{
    return _register;
}

shared_ptr<DebugMessage> WriteRegisterCommand::EncodeRequestMessage()
{
    auto buffer = make_shared<std::array<uint8_t, MaxMessageBufferSize>>();
    (*buffer)[0] = static_cast<uint16_t>(MessageID::MessageWriteRegister) & 0xFF;
    (*buffer)[1] = ((static_cast<uint16_t>(MessageID::MessageWriteRegister)) >> 0x08) & 0xFF;
    (*buffer)[2] = static_cast<uint8_t>(_register);
    (*buffer)[3] = _registerValue & 0xff;
    (*buffer)[4] = (_registerValue >> 0x08) & 0xff;

    return make_shared<DebugMessage>(buffer);
}

}