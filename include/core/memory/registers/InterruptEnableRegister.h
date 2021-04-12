#pragma once

#include "MemoryMappedRegister.h"

namespace gbxcore::memory::registers
{

class InterruptEnableRegister : public gbxcore::interfaces::MemoryMappedRegister
{
public:
    InterruptEnableRegister() = default;
    virtual ~InterruptEnableRegister() = default;
    
    std::size_t Size() override;
    std::variant<uint8_t, uint16_t> Read(size_t, interfaces::MemoryAccessType) override;
    void Write(std::variant<uint8_t, uint16_t>, size_t) override;
};

}