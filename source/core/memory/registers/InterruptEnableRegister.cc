#include "InterruptEnableRegister.h"

using namespace std;
using namespace gbxcore::interfaces;

namespace gbxcore::memory::registers
{

size_t InterruptEnableRegister::Size()
{
    return {};
}

variant<uint8_t, uint16_t> InterruptEnableRegister::Read(size_t, MemoryAccessType)
{
    return {};
}

void InterruptEnableRegister::Write(variant<uint8_t, uint16_t>, size_t)
{}

}