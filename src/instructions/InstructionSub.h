#pragma once

#include <memory>
#include <optional>

#include "../interfaces/InstructionInterface.h"
#include "InstructionSubBase.h"

namespace gbx
{

class InstructionSub : public interfaces::InstructionInterface, public InstructionSubBase
{
public:
    InstructionSub() = default;
    virtual ~InstructionSub() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&, bool&) override;

private:
    inline void DecodeSubRegisterMode(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeSubImmediateMode(interfaces::DecodedInstruction&);
    inline void DecodeSubRegisterIndirectMode(interfaces::DecodedInstruction&);
};

}