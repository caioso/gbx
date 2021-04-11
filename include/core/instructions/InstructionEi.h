#pragma once

#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionEi : public interfaces::InstructionInterface
{
public:
    InstructionEi() = default;
    virtual ~InstructionEi() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&) override;
};

}