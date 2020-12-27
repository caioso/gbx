#pragma once

#include <memory>

#include "../GBXExceptions.h"
#include "Instruction.h"

namespace gbx
{

class LD : public Instruction
{
public:
    LD() = default;
    virtual ~LD() = default;
    virtual void Decode(uint8_t opcode) override;
    virtual void Execute(std::shared_ptr<RegisterBank> registerBank, std::shared_ptr<Channel<MemoryMessage>> memoryChannel) override;
};

}