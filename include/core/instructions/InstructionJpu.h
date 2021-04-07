#pragma once

#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionJpu : public interfaces::InstructionInterface
{
public:
    InstructionJpu() = default;
    virtual ~InstructionJpu() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&) override;
};

}