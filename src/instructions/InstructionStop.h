#pragma once

#include "../interfaces/InstructionInterface.h"
#include "../GBXExceptions.h"

namespace gbx
{

class InstructionStop : public interfaces::InstructionInterface
{
public:
    InstructionStop() = default;
    virtual ~InstructionStop() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&) override;
};

}