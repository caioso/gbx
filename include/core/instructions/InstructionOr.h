#pragma once

#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionOr : public interfaces::InstructionInterface
{
public:
    InstructionOr() = default;
    virtual ~InstructionOr() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&) override;

private:
    inline void DecodeOrRegisterMode(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeOrImmediateMode(interfaces::DecodedInstruction&);
    inline void DecodeOrRegisterIndirectMode(interfaces::DecodedInstruction&);
    
    inline uint8_t GetSourceOperandValue(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
};

}