#pragma once

#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionCcf : public interfaces::InstructionInterface
{
public:
    InstructionCcf() = default;
    virtual ~InstructionCcf() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&) override;
};

}