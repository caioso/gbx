#pragma once

namespace gbx
{

enum class OpcodeType
{
    unknown,
    ld,
    add
};

const uint8_t MemoryOperand = 0x06;

}