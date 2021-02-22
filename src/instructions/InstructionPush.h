#pragma once

#include "../interfaces/InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionPush : public interfaces::InstructionInterface
{
public:
    InstructionPush() = default;
    virtual ~InstructionPush() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&) override;
};

}