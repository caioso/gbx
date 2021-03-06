#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <variant>

namespace gbxcore::interfaces
{

enum class MemoryAccessType
{
    Byte,
    Word
};

class MemoryResource
{
public:
    virtual ~MemoryResource() = default;
    virtual std::size_t Size() = 0;
    virtual std::variant<uint8_t, uint16_t> Read(size_t, MemoryAccessType) = 0;
    virtual void Write(std::variant<uint8_t, uint16_t>, size_t) = 0;
    virtual void Load(std::shared_ptr<uint8_t*>, std::size_t, std::optional<size_t>) = 0;
};

}