#pragma once

#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionBit : public interfaces::InstructionInterface
{
public:
    InstructionBit() = default;
    virtual ~InstructionBit() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&) override;

private:
    inline void DecodeBitRegisterMode(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeBitRegisterIndirectMode(uint8_t, interfaces::DecodedInstruction&);
    inline void SetFlags(uint8_t, interfaces::RegisterBankInterface*);
    inline uint8_t AcquireOperand(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&);
};

}