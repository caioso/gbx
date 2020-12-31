#pragma once

#include <optional>
#include <variant>

#include "AddressRange.h"
#include "Memory.h"

namespace gbx
{

enum class MemoryRequestType
{
    Read,
    Write,
    Result
};

class MemoryControllerInterface
{
public:
    virtual std::variant<uint8_t, uint16_t> Read(uint16_t, MemoryAccessType) = 0;
    virtual void Write(std::variant<uint8_t, uint16_t>, uint16_t) = 0;
    virtual void Load(std::shared_ptr<uint8_t*>, size_t, uint16_t, std::optional<size_t>) = 0;

    virtual void RegisterMemoryResource(std::shared_ptr<Memory>, AddressRange) = 0;
    virtual void UnregisterMemoryResource(std::shared_ptr<Memory>) = 0;
};

}