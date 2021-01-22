#pragma once

#include "../interfaces/InstructionInterface.h"

namespace gbx
{

class InstructionRrc : public interfaces::InstructionInterface
{
public:
    InstructionRrc() = default;
    virtual ~InstructionRrc() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&) override;

private:
    inline void DecodeRrcRegisterMode(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeRrcRegisterIndirectMode(interfaces::DecodedInstruction&);
    inline void SetFlags(uint8_t, std::shared_ptr<interfaces::RegisterBankInterface>, uint8_t);
    inline uint8_t AcquireOperand(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    inline void WriteResult(uint8_t, std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
};

}