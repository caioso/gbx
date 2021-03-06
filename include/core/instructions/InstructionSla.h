#pragma once

#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionSla : public interfaces::InstructionInterface
{
public:
    InstructionSla() = default;
    virtual ~InstructionSla() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&) override;

private:
    inline void DecodeSlaRegisterMode(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeSlaRegisterIndirectMode(interfaces::DecodedInstruction&);
    inline void SetFlags(uint8_t, interfaces::RegisterBankInterface*, uint8_t);
    inline uint8_t AcquireOperand(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&);
    inline void WriteResult(uint8_t, interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&);
};

}