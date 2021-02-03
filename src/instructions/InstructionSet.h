#pragma once

#include "../interfaces/InstructionInterface.h"

namespace gbx
{

class InstructionSet : public interfaces::InstructionInterface
{
public:
    InstructionSet() = default;
    virtual ~InstructionSet() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&, bool&) override;

private:
    inline void DecodeSetRegisterMode(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeSetRegisterIndirectMode(uint8_t, interfaces::DecodedInstruction&);
    inline uint8_t AcquireOperand(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    inline void WriteResult(uint8_t, std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
};

}