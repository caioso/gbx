#pragma once

#include "../interfaces/InstructionInterface.h"

namespace gbx
{

class InstructionAdd : public interfaces::InstructionInterface
{
public:
    InstructionAdd() = default;
    virtual ~InstructionAdd() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<RegisterBank>, interfaces::DecodedInstruction&) override;

private:
    inline void DecodeAddRegisterMode(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeAddImmediateMode(interfaces::DecodedInstruction&);

    inline uint8_t AcquireSourceOperandValue(std::shared_ptr<RegisterBank>, interfaces::DecodedInstruction&);
    inline uint8_t CalculateBinaryAdditionAndSetFlags(uint8_t, uint8_t, std::shared_ptr<RegisterBank>);
};

}