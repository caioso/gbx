#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <variant>

namespace gbx
{

enum class MemoryAccessType
{
    Byte,
    Word
};

class Memory
{
public:
    virtual void Load(const uint8_t* content, std::size_t size, std::optional<size_t> offset) = 0;

    virtual std::size_t Size() = 0;

    virtual std::variant<uint8_t, uint16_t> Read(uint16_t address, MemoryAccessType accessType) = 0;
    virtual void Write(std::variant<uint8_t, uint16_t> value, uint16_t address) = 0;
};

}