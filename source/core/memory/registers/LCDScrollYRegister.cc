#include "LCDScrollYRegister.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::memory::registers
{

LCDScrollYRegister::LCDScrollYRegister(VideoControllerInterface* controller)
    : _videoController(controller)
{}
    
size_t LCDScrollYRegister::Size()
{
    return 1;
}

variant<uint8_t, uint16_t> LCDScrollYRegister::Read()
{
    return _value;
}

void LCDScrollYRegister::Write(variant<uint8_t, uint16_t> value)
{
    _value = Extract8BitValue(value);
    _videoController->ScrollBackgroundY(_value);
}

inline uint8_t LCDScrollYRegister::Extract8BitValue(std::variant<uint8_t, uint16_t> value)
{
    if (holds_alternative<uint16_t>(value))
        return static_cast<uint8_t>(get<uint16_t>(value) & 0xFF);
    else
        return get<uint8_t>(value);
}

}