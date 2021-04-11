#pragma once

#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionScf : public interfaces::InstructionInterface
{
public:
    InstructionScf() = default;
    virtual ~InstructionScf() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&) override;
};

}