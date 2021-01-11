#pragma once

namespace gbx
{

enum class OpcodeType
{
    unknown,
    ld,
    add,
    adc,
    sub,
    sbc,
    bitwiseand,
    bitwiseor,
    bitwisexor,
    cp,
};

const uint8_t MemoryOperand = 0x06;

}