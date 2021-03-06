#pragma once

#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionRrca : public interfaces::InstructionInterface
{
public:
    InstructionRrca() = default;
    virtual ~InstructionRrca() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&) override;

private:
    inline void SetFlags(interfaces::RegisterBankInterface*, uint8_t);
};

}