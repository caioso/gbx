#include "LCDStatusRegister.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::memory::registers
{

LCDStatusRegister::LCDStatusRegister(VideoControllerInterface* controller)
    : _videoController(controller)
{}

std::size_t LCDStatusRegister::Size()
{
    return 1;
}

std::variant<uint8_t, uint16_t> LCDStatusRegister::Read()
{
    return _value;
}
    
void LCDStatusRegister::Write(std::variant<uint8_t, uint16_t> value)
{
    auto originalValue = _value & 0x07; // save bits 0, 1, 2
    auto valueToBeWritten = Extract8BitValue(value);;
    DecodeRegisterValue(valueToBeWritten, originalValue);

    // Update Interrupt Modes
    _videoController->SetInterruptMode((valueToBeWritten >> 0x03) & 0x0F);
}

inline uint8_t LCDStatusRegister::Extract8BitValue(std::variant<uint8_t, uint16_t> value)
{
    if (holds_alternative<uint16_t>(value))
        return static_cast<uint8_t>(get<uint16_t>(value) & 0xFF);
    else
        return get<uint8_t>(value);
}

inline void LCDStatusRegister::DecodeRegisterValue(uint8_t valueToBeWritten, uint8_t originalValue)
{
    valueToBeWritten &= 0x78;
    _value = (originalValue | valueToBeWritten) & 0x7F;
}

}