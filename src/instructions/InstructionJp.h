#pragma once

#include "../interfaces/InstructionInterface.h"

namespace gbx
{

class InstructionJp : public interfaces::InstructionInterface
{
public:
    InstructionJp() = default;
    virtual ~InstructionJp() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&, bool&) override;

private:
    inline void DecodeUnconditionalJpRegisterIndirect(interfaces::DecodedInstruction&);
    inline void DecodeUnconditionalJpImmediate(interfaces::DecodedInstruction&);
    inline void DecodeConditionalJp(uint8_t, interfaces::DecodedInstruction&);
    inline void ExecuteUnconditionalJp(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    inline void ExecuteUnconditionalJpRegisterIndirect(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    inline void ExecuteConditionalJp(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
};

}