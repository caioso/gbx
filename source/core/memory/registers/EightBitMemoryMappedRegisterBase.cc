#include "EightBitMemoryMappedRegisterBase.h"

using namespace std;

namespace gbxcore::memory::registers
{

size_t EightBitMemoryMappedRegisterBase::Size()
{
    return 1;
}

std::variant<uint8_t, uint16_t> EightBitMemoryMappedRegisterBase::Read()
{
    return _value;
}

void EightBitMemoryMappedRegisterBase::Write(std::variant<uint8_t, uint16_t> value)
{
    _value = Extract8BitValue(value);
}

}