#pragma once

#include "../interfaces/InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionDaa : public interfaces::InstructionInterface
{
public:
    InstructionDaa() = default;
    virtual ~InstructionDaa() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&) override;

private:
    uint8_t ExecuteSubtractionDaa(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    uint8_t ExecuteAdditionDaa(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
};

}