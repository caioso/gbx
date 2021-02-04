#pragma once

#include "../interfaces/InstructionInterface.h"

namespace gbx
{

class InstructionCall : public interfaces::ConditionalInstructionInterface
{
public:
    InstructionCall() = default;
    virtual ~InstructionCall() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual bool ConditionallyExecute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&) override;

private:
    inline void DecodeUnconditionalCall(interfaces::DecodedInstruction&);
    inline void DecodeConditionalCall(uint8_t, interfaces::DecodedInstruction&);
    inline bool ExecuteUnconditionalCall(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    inline bool ExecuteConditionalCall(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
};

}