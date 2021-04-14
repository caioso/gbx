#pragma once 

#include <cstdint>
#include <memory>
#include <optional>
#include <variant>

#include "MemoryResource.h"

namespace gbxcore::interfaces
{

class MemoryMappedRegister
{
public:
    virtual ~MemoryMappedRegister() = default;
    virtual std::size_t Size() = 0;
    virtual std::variant<uint8_t, uint16_t> Read() = 0;
    virtual void Write(std::variant<uint8_t, uint16_t>) = 0;
};

}