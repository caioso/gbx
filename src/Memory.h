#pragma once

#include <cstdint>

namespace gbx
{

class Memory
{
public:
    virtual uint8_t ReadByte(uint16_t address) = 0;
    virtual uint16_t ReadWord(uint16_t address) = 0;

    virtual void WriteByte(uint8_t value, uint16_t address) = 0;
    virtual void WriteWord(uint16_t value, uint16_t address) = 0;
};

}