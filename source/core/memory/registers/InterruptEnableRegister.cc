#include "InterruptEnableRegister.h"

using namespace std;
using namespace gbxcore::interfaces;

namespace gbxcore::memory::registers
{

size_t InterruptEnableRegister::Size()
{
    return 1llu;
}

variant<uint8_t, uint16_t> InterruptEnableRegister::Read()
{
    return _value;
}

void InterruptEnableRegister::Write(variant<uint8_t, uint16_t> value)
{
    if (holds_alternative<uint16_t>(value))
        _value = get<uint16_t>(value) & 0xFF;
    else
        _value = get<uint8_t>(value);
}

}