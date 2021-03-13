#pragma once

#include <memory>
#include <optional>

#include "../interfaces/InstructionInterface.h"
#include "InstructionSubBase.h"

namespace gbxcore::instructions
{

class InstructionSbc : public interfaces::InstructionInterface, public InstructionSubBase
{
public:
    InstructionSbc() = default;
    virtual ~InstructionSbc() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&) override;

private:
    inline void DecodeSbcRegisterMode(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeSbcImmediateMode(interfaces::DecodedInstruction&);
    inline void DecodeSbcRegisterIndirectMode(interfaces::DecodedInstruction&);
};

}