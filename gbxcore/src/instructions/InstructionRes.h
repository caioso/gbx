#pragma once

#include "../interfaces/InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionRes : public interfaces::InstructionInterface
{
public:
    InstructionRes() = default;
    virtual ~InstructionRes() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&) override;

private:
    inline void DecodeResRegisterMode(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeResRegisterIndirectMode(uint8_t, interfaces::DecodedInstruction&);
    inline uint8_t AcquireOperand(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    inline void WriteResult(uint8_t, std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
};

}