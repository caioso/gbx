#pragma once

namespace gbxcore::instructions
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
    ei,
    di,
    ccf,
    scf,

    // GBX ONLY
    // System SecurityLevel Operations
    jpu,
    ldu,
};

const uint8_t MemoryOperand = 0x06;

}