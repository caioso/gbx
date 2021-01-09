#pragma once

namespace gbx
{

enum class OpcodeType
{
    unknown,
    ld,
    add,
    adc,
    sub
};

const uint8_t MemoryOperand = 0x06;

}