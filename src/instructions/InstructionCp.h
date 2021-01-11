#pragma once

#include "InstructionAddBase.h"
#include "../interfaces/InstructionInterface.h"

namespace gbx
{

class InstructionCp : public interfaces::InstructionInterface
{
public:
    InstructionCp() = default;
    virtual ~InstructionCp() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&) override;

private:
    inline void DecodeCpRegisterMode(uint8_t, interfaces::DecodedInstruction&);
    
    inline void CalculateDifference(uint8_t, uint8_t, std::shared_ptr<interfaces::RegisterBankInterface>);
    inline uint8_t GetSourceOperandValue(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
};

}