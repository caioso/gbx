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
    virtual void Decode(uint8_t, std::optional<uint8_t> preOpcode) override;
    virtual void Execute(std::shared_ptr<RegisterBank>, std::shared_ptr<Channel<MemoryMessage>>) override;

private:
    inline void DecodeImmediateOperand(uint8_t);
    inline void DecodeRegisterIndirectOperandBCDE(uint8_t);
    inline void DecodeRegisterIndirectOperandHL(uint8_t);
    inline void DecodeRegisterIndirectOperandDestinationHL(uint8_t);
    inline void DecodeRegisterRegisterOperand(uint8_t);
    inline void DecodeRegisterIndirectOperandDestinationBCDE(uint8_t);
    inline void DecodeRegisterIndexedSource(uint8_t, uint8_t);

    inline void ExecuteOneOperandBasedAddressingMode(std::shared_ptr<RegisterBank>);
    inline void ExecuteTwoOperandsBasedAddressingMode(std::shared_ptr<RegisterBank>);
    inline void ExecuteRegisterMode(std::shared_ptr<RegisterBank>);
    inline void ExecuteRegisterIndirectDestinationMode(std::shared_ptr<RegisterBank>);

};

}