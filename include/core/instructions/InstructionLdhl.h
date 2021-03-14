#pragma once

#include "InstructionAddBase.h"
#include "../interfaces/InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionLdhl : public interfaces::InstructionInterface, public InstructionAddBase
{
public:
    InstructionLdhl() = default;
    virtual ~InstructionLdhl() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&) override;
};

}