#pragma once

#include <iostream>

#include "InstructionAddBase.h"
#include "InstructionInterface.h"

namespace gbxcore::instructions
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
    inline void DecodeIncRegisterPairMode(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeIncRegisterIndirectMode(interfaces::DecodedInstruction&);
    
    inline void Execute8bitIncrement(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    inline void Execute16bitIncrement(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);

    inline void SetDestinationOperandValue(uint8_t, std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
};

}