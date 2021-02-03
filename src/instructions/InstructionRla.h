#pragma once

#include "../interfaces/InstructionInterface.h"

namespace gbx
{

class InstructionRla : public interfaces::InstructionInterface
{
public:
    InstructionRla() = default;
    virtual ~InstructionRla() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&, bool&) override;

private:
    inline void SetFlags(std::shared_ptr<interfaces::RegisterBankInterface>, uint8_t);
};

}