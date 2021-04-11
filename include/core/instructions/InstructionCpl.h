#pragma once

#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionCpl : public interfaces::InstructionInterface
{
public:
    InstructionCpl() = default;
    virtual ~InstructionCpl() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&) override;
};

}