#pragma once

#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionDaa : public interfaces::InstructionInterface
{
public:
    InstructionDaa() = default;
    virtual ~InstructionDaa() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&) override;

private:
    uint8_t ExecuteSubtractionDaa(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&);
    uint8_t ExecuteAdditionDaa(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&);
};

}