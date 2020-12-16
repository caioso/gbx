#pragma once

#include <cstdint>
#include <memory>
#include <optional>

namespace gbx
{

class Memory
{
public:
    virtual void Load(const uint8_t* content, std::size_t size, std::optional<size_t> offset) = 0;

    virtual std::size_t Size() = 0;

    virtual uint8_t ReadByte(uint16_t address) = 0;
    virtual uint16_t ReadWord(uint16_t address) = 0;

    virtual void WriteByte(uint8_t value, uint16_t address) = 0;
    virtual void WriteWord(uint16_t value, uint16_t address) = 0;
};

}