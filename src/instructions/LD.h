#pragma once

#include "../GBXExceptions.h"
#include "Instruction.h"

namespace gbx
{

class LD : public Instruction
{
public:
    virtual void Decode(uint8_t opcode) override;
    virtual void Execute(RegisterBank registerBank, std::shared_ptr<Channel<MemoryMessage>> memoryChannel) override;
};

}