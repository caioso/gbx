#pragma once

#include "InstructionAddBase.h"
#include "../interfaces/InstructionInterface.h"

namespace gbx
{

class InstructionInc : public interfaces::InstructionInterface, public InstructionAddBase
{
public:
    InstructionInc() = default;
    virtual ~InstructionInc() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&) override;

private:
    inline void DecodeIncRegisterMode(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeIncRegisterIndirectMode(interfaces::DecodedInstruction&);
    
    inline void SetDestinationOperandValue(uint8_t, std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
};

}