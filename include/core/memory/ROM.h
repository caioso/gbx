#pragma once

#include <algorithm>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <variant>

#include "GBXCoreExceptions.h"
#include "MemoryInterface.h"

namespace gbxcore::memory
{
    
class ROM : public interfaces::MemoryInterface
{
public:
    ROM(std::size_t);
    virtual ~ROM() = default;

    virtual std::variant<uint8_t, uint16_t> Read(size_t, interfaces::MemoryAccessType) override;
    virtual void Write(std::variant<uint8_t, uint16_t>, size_t) override;

    virtual std::size_t Size() override;
    virtual void Load(std::shared_ptr<uint8_t*>, std::size_t, std::optional<size_t>) override;

protected:
    inline void CheckReadConditions(size_t, interfaces::MemoryAccessType);

    std::size_t _size;
    std::unique_ptr<uint8_t[]> _rom;
};

}