#pragma once

#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionAnd : public interfaces::InstructionInterface
{
public:
    InstructionAnd() = default;
    virtual ~InstructionAnd() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&) override;

private:
    inline void DecodeAddRegisterMode(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeImmediateMode(interfaces::DecodedInstruction&);
    inline void DecodeRegisterIndirectMode(interfaces::DecodedInstruction&);

    inline uint8_t GetSourceOperandValue(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
};

}