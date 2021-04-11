#pragma once

#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionJr : public interfaces::InstructionInterface
{
public:
    InstructionJr() = default;
    virtual ~InstructionJr() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&) override;

private:
    inline void DecodeUnconditionalJr(interfaces::DecodedInstruction&);
    inline void DecodeConditionalJr(uint8_t, interfaces::DecodedInstruction&);
    inline void ExecuteUnconditionalJr(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&);
    inline void ExecuteConditionalJr(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&);
};

}