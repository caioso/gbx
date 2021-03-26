#pragma once

#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionRet : public interfaces::InstructionInterface
{
public:
    InstructionRet() = default;
    virtual ~InstructionRet() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&) override;

private:
    inline void DecodeConditionalRet(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&);
    inline void DecodeUnconditionalRet(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&);
    inline void ExecuteConditionalRet(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    inline void ExecuteUnconditionalRet(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
};

}