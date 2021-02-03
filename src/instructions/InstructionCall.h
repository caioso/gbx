#pragma once

#include "../interfaces/InstructionInterface.h"

namespace gbx
{

class InstructionCall : public interfaces::InstructionInterface
{
public:
    InstructionCall() = default;
    virtual ~InstructionCall() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&, bool&) override;

private:
    inline void DecodeUnconditionalCall(interfaces::DecodedInstruction&);
    inline void DecodeConditionalCall(uint8_t, interfaces::DecodedInstruction&);
    inline void ExecuteUnconditionalCall(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    inline void ExecuteConditionalCall(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&, bool&);
};

}