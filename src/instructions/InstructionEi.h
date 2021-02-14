#pragma once

#include "../interfaces/InstructionInterface.h"

namespace gbx
{

class InstructionEi : public interfaces::InstructionInterface
{
public:
    InstructionEi() = default;
    virtual ~InstructionEi() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&) override;
};

}