#pragma once

#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionCp : public interfaces::InstructionInterface
{
public:
    InstructionCp() = default;
    virtual ~InstructionCp() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&) override;

private:
    inline void DecodeCpRegisterMode(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeCpImmediateMode(interfaces::DecodedInstruction&);
    inline void DecodeCpRegisterIndirectMode(interfaces::DecodedInstruction&);
    
    inline void CalculateDifferenceAndSetFlags(uint8_t, uint8_t, interfaces::RegisterBankInterface*);
    inline uint8_t GetSourceOperandValue(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&);
};

}