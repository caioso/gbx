#pragma once

#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionSet : public interfaces::InstructionInterface
{
public:
    InstructionSet() = default;
    virtual ~InstructionSet() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&) override;

private:
    inline void DecodeSetRegisterMode(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeSetRegisterIndirectMode(uint8_t, interfaces::DecodedInstruction&);
    inline uint8_t AcquireOperand(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&);
    inline void WriteResult(uint8_t, interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&);
};

}