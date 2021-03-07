#include "ReadRegisterCommand.h"

using namespace gbx::interfaces;
using namespace gbxcore::interfaces;
using namespace std;

namespace gbx::protocol
{
ReadRegisterCommand::ReadRegisterCommand()
    : DebugCommand(static_cast<uint16_t>(CommandID::CommandReadRegister))
{}

gbxcore::interfaces::Register ReadRegisterCommand::RegisterToRead()
{
    return _register;
}

void ReadRegisterCommand::DecodeRequestMessage(std::shared_ptr<DebugMessage> message)
{
    // Move this to a "Parse Method" in the ReadRegisterCommand class
    auto targetRegister = (*message->Buffer())[2];

    auto validRegisters = {Register::B, Register::C, Register::D, Register::E, Register::H, Register::L,
                        Register::A, Register::F, Register::IR, Register::PIR, Register::R, Register::PC,
                        Register::SP, Register::IX, Register::IY,  Register::HL, Register::BC, Register::DE, Register::AF};

    for (auto reg : validRegisters)
        if (static_cast<uint8_t>(reg) == targetRegister)
        {
            _register = static_cast<gbxcore::interfaces::Register>(targetRegister);
            return;
        }

    throw MessageHandlerException("Invalid target register found when parsing 'ReadRegister' command");
}

std::shared_ptr<DebugMessage> ReadRegisterCommand::EncodeRequestMessage()
{
    auto buffer = make_shared<std::array<uint8_t, MaxMessageBufferSize>>();
    (*buffer)[0] = 0xFE;
    (*buffer)[1] = 0xFF;
    (*buffer)[2] = _registerValue & 0xff;
    (*buffer)[3] = (_registerValue >> 0x08) & 0xff;

    return make_shared<DebugMessage>(buffer);
}

void ReadRegisterCommand::SetRegisterValue(uint16_t registerValue)
{
    _registerValue = registerValue;
}

}