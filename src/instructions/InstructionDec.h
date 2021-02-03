#pragma once

#include "../interfaces/InstructionInterface.h"
#include "InstructionSubBase.h"

namespace gbx
{

class InstructionDec : public interfaces::InstructionInterface, public InstructionSubBase
{
public:
    InstructionDec() = default;
    virtual ~InstructionDec() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&, bool&) override;

private:
    inline void DecodeDecRegisterMode(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeDecRegisterPairMode(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeDecRegisterIndirectMode(interfaces::DecodedInstruction&);
    
    void Execute8bitDecrement(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    void Execute16bitDecrement(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);

    inline void SetDestinationOperandValue(uint8_t, std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
};

}