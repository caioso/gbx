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
    inc,
    dec,
    push,
    pop,
    ldhl,
    rlca,
    rla,
    rrca,
    rra,
    rlc,
    rl,
};

const uint8_t MemoryOperand = 0x06;

}