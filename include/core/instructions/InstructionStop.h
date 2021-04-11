#pragma once

#include "InstructionInterface.h"
#include "GBXCoreExceptions.h"

namespace gbxcore::instructions
{

class InstructionStop : public interfaces::InstructionInterface
{
public:
    InstructionStop() = default;
    virtual ~InstructionStop() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&) override;
};

}