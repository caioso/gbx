#pragma once

#include "../interfaces/InstructionInterface.h"
#include "InstructionAddBase.h"

namespace gbx
{

class InstructionAdc : public interfaces::InstructionInterface, public InstructionAddBase
{
public:
    InstructionAdc() = default;
    virtual ~InstructionAdc() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&) override;

private:
    void DecodeAddRegisterMode(uint8_t, interfaces::DecodedInstruction&);
    void DecodeAddImmediateMode(interfaces::DecodedInstruction&);
    void DecodeAddRegisterIndirectMode(interfaces::DecodedInstruction&);
};

}