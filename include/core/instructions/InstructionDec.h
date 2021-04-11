#pragma once

#include "InstructionInterface.h"
#include "InstructionSubBase.h"

namespace gbxcore::instructions
{

class InstructionDec : public interfaces::InstructionInterface, public InstructionSubBase
{
public:
    InstructionDec() = default;
    virtual ~InstructionDec() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&) override;

private:
    inline void DecodeDecRegisterMode(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeDecRegisterPairMode(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeDecRegisterIndirectMode(interfaces::DecodedInstruction&);
    
    void Execute8bitDecrement(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&);
    void Execute16bitDecrement(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&);

    inline void SetDestinationOperandValue(uint8_t, interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&);
};

}