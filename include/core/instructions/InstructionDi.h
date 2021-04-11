#pragma once

#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionDi : public interfaces::InstructionInterface
{
public:
    InstructionDi() = default;
    virtual ~InstructionDi() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&) override;
};

}