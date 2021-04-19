#include "LCDStatusRegister.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::memory::registers
{

LCDStatusRegister::LCDStatusRegister(VideoControllerInterface* controller)
    : _videoController(controller)
{}

void LCDStatusRegister::Write(std::variant<uint8_t, uint16_t> value)
{
    auto originalValue = _value & 0x07; // save bits 0, 1, 2
    auto valueToBeWritten = Extract8BitValue(value);;
    DecodeRegisterValue(valueToBeWritten, originalValue);

    // Update Interrupt Modes
    _videoController->SetInterruptMode((valueToBeWritten >> 0x03) & 0x0F);
}

inline void LCDStatusRegister::DecodeRegisterValue(uint8_t valueToBeWritten, uint8_t originalValue)
{
    valueToBeWritten &= 0x78;
    _value = (originalValue | valueToBeWritten) & 0x7F;
}

}