#pragma once

#include <memory>

#include "AddressRange.h"
#include "Memory.h"

namespace gbx
{

class MemoryController
{
public:
    MemoryController() = default;
    ~MemoryController() = default;

    void WriteByte(uint8_t value, uint16_t address);
    void WriteWord(uint16_t value, uint16_t address);
    uint8_t ReadByte(uint16_t address);
    uint16_t ReadWord(uint16_t address);

    void RegisterMemoryResource(std::shared_ptr<Memory> resource, AddressRange range);
    void UnregisterMemoryResource(std::shared_ptr<Memory> resource);
};

}