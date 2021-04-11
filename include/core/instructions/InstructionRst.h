#pragma once

#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionRst : public interfaces::InstructionInterface
{
public:
    InstructionRst() = default;
    virtual ~InstructionRst() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&) override;

private:
    uint8_t GetPageLowerAddress(uint8_t page);
};

}