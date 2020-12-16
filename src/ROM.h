#pragma once

#include <algorithm>
#include <cstdlib>
#include <cstdint>
#include <memory>
#include <optional>
#include <sstream>

#include "GBXExceptions.h"
#include "Memory.h"

namespace gbx
{

class ROM : public Memory
{
public:
    ROM(std::size_t sizeInBytes);
    virtual ~ROM() = default;

    virtual std::size_t Size() override;
    virtual void Load(const uint8_t* content, std::size_t size, std::optional<size_t> offset) override;
    virtual uint8_t ReadByte(uint16_t address) override;
    virtual uint16_t ReadWord(uint16_t address) override ;
    virtual void WriteByte(uint8_t value, uint16_t address) override;
    virtual void WriteWord(uint16_t value, uint16_t address) override;

private:
    std::size_t _size;
    std::unique_ptr<uint8_t[]> _rom;
};

}