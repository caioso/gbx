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
    rrc,
    rr,
    sla,
    sra,
    srl,
    swap,
    bit,
    set,
    res,
    jp,
    jr,
    call,
    ret,
    reti,
    rst,
    daa,
    cpl,
    nop,
    halt,
    stop,
};

const uint8_t MemoryOperand = 0x06;

}