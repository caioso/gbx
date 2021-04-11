#pragma once

#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionReti : public interfaces::InstructionInterface
{
public:
    InstructionReti() = default;
    virtual ~InstructionReti() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&) override;
};

}