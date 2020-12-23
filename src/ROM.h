#pragma once

#include <algorithm>
#include <cstdlib>
#include <cstdint>
#include <memory>
#include <optional>
#include <sstream>
#include <variant>

#include "Channel.h"
#include "GBXExceptions.h"
#include "Memory.h"

namespace gbx
{
    
class ROM : public Memory
{
public:
    ROM(std::size_t sizeInBytes);
    virtual ~ROM() = default;

    virtual std::variant<uint8_t, uint16_t> Read(uint16_t address, MemoryAccessType accessType) override;
    virtual void Write(std::variant<uint8_t, uint16_t> value, uint16_t address) override;

    virtual std::size_t Size() override;
    virtual void Load(std::shared_ptr<uint8_t*> content, std::size_t size, std::optional<size_t> offset) override;

private:
    void CheckReadConditions(uint16_t address, MemoryAccessType accessType);
    void CheckWriteConditions(std::variant<uint8_t, uint16_t> value, uint16_t address);
    std::size_t _size;
    std::unique_ptr<uint8_t[]> _rom;
};

}