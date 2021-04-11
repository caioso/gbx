#pragma once

#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionPop : public interfaces::InstructionInterface
{
public:
    InstructionPop() = default;
    virtual ~InstructionPop() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&) override;
};

}