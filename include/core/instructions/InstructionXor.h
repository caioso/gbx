#pragma once

#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionXor : public interfaces::InstructionInterface
{
public:
    InstructionXor() = default;
    virtual ~InstructionXor() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&) override;

private:
    inline void DecodeXorRegisterMode(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeXorImmediateMode(interfaces::DecodedInstruction&);
    inline void DecodeXorRegisterIndirectMode(interfaces::DecodedInstruction&);

    inline uint8_t GetSourceOperandValue(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
};

}