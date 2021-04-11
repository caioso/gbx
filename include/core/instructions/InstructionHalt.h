#pragma once

#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionHalt : public interfaces::InstructionInterface
{
public:
    InstructionHalt() = default;
    virtual ~InstructionHalt() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&) override;
};

}