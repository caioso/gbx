#pragma once

#include <stdlib.h>
#include <variant>

#include "MemoryMappedRegister.h"

namespace gbxcore::memory::registers
{

class EightBitMemoryMappedRegisterBase : public gbxcore::interfaces::MemoryMappedRegister
{
public:
    EightBitMemoryMappedRegisterBase() = default;
    virtual ~EightBitMemoryMappedRegisterBase() = default;

    inline uint8_t Extract8BitValue(std::variant<uint8_t, uint16_t> value)
    {
        if (std::holds_alternative<uint16_t>(value))
            return static_cast<uint8_t>(std::get<uint16_t>(value) & 0xFF);
        else
            return std::get<uint8_t>(value);
    }

    virtual size_t Size();
    virtual std::variant<uint8_t, uint16_t> Read();
    virtual void Write(std::variant<uint8_t, uint16_t>);

protected:
    uint8_t _value{};
};

}