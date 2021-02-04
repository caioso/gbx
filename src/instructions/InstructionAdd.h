#pragma once

#include "InstructionAddBase.h"
#include "../interfaces/InstructionInterface.h"

namespace gbx
{

class InstructionAdd : public interfaces::InstructionInterface, public InstructionAddBase
{
public:
    InstructionAdd() = default;
    virtual ~InstructionAdd() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&) override;

private:
    inline void Add8Bit(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    inline void Add16Bit(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    inline void DecodeAddRegisterMode(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeAddRegisterPairMode(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeAddImmediateMode(interfaces::DecodedInstruction&);
    inline void DecodeAddImmediatePairMode(interfaces::DecodedInstruction&);
    inline void DecodeAddPointerMode(interfaces::DecodedInstruction&);
};

}