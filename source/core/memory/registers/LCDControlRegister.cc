#include "LCDControlRegister.h"

using namespace gbxcore;
using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::memory::registers
{

LCDControlRegister::LCDControlRegister(VideoControllerInterface* controller)
    : _videoController(controller)
{}

size_t LCDControlRegister::Size()
{
    return 1llu;
}

variant<uint8_t, uint16_t> LCDControlRegister::Read()
{
    return _value;
}

void LCDControlRegister::Write(variant<uint8_t, uint16_t> value)
{ 
    auto oldValue = _value;
    if (holds_alternative<uint16_t>(value))
        _value = get<uint16_t>(value) & 0xFF;
    else
        _value = get<uint8_t>(value);

    ProcessValue(oldValue);
}

void LCDControlRegister::ProcessValue(uint8_t oldValue)
{
    // Check MSBIT (LDC Enable/Disable)
    if (oldValue & 0x80 || _value & 0x80)
        HandleLCDControBit(_value >> 0x07);
}

void LCDControlRegister::HandleLCDControBit(uint8_t bit)
{
    if (bit == 0x01)
        _videoController->EnableVideo();
    else
        _videoController->DisableVideo();

}

}