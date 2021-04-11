#pragma once

#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionCall : public interfaces::ConditionalInstructionInterface
{
public:
    InstructionCall() = default;
    virtual ~InstructionCall() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual bool ConditionallyExecute(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&) override;

private:
    inline void DecodeUnconditionalCall(interfaces::DecodedInstruction&);
    inline void DecodeConditionalCall(uint8_t, interfaces::DecodedInstruction&);
    inline bool ExecuteUnconditionalCall(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&);
    inline bool ExecuteConditionalCall(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&);
};

}