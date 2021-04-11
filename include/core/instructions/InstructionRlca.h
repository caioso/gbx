#pragma once

#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionRlca : public interfaces::InstructionInterface
{
public:
    InstructionRlca() = default;
    virtual ~InstructionRlca() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&) override;

private:
    inline void SetFlags(interfaces::RegisterBankInterface*, uint8_t);
};

}