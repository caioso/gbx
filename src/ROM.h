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
    ROM(std::size_t);
    virtual ~ROM() = default;

    virtual std::variant<uint8_t, uint16_t> Read(uint16_t, MemoryAccessType) override;
    virtual void Write(std::variant<uint8_t, uint16_t>, uint16_t) override;

    virtual std::size_t Size() override;
    virtual void Load(std::shared_ptr<uint8_t*>, std::size_t, std::optional<size_t>) override;

private:
    inline void CheckReadConditions(uint16_t, MemoryAccessType);
    inline void CheckWriteConditions(std::variant<uint8_t, uint16_t>, uint16_t);
    
    std::size_t _size;
    std::unique_ptr<uint8_t[]> _rom;
};

}