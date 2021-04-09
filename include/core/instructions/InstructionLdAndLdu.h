#pragma once

#include "InstructionUtilities.h"
#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionLdAndLdu : public interfaces::InstructionInterface
{
public:
    InstructionLdAndLdu() = default;
    virtual ~InstructionLdAndLdu() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&) override;

private:
    inline void DecodeRegisterRegisterMove(uint8_t, interfaces::DecodedInstruction&, std::optional<uint8_t>);
    inline void DecodeRegisterImmediateMove(uint8_t, interfaces::DecodedInstruction&, std::optional<uint8_t>);
    inline void DecodeRegisterPointerMove(uint8_t, interfaces::DecodedInstruction&, std::optional<uint8_t>);
    inline void DecodePointerRegisterMove(uint8_t, interfaces::DecodedInstruction&, std::optional<uint8_t>);
    inline void DecodePointerImmediateMove(interfaces::DecodedInstruction&, std::optional<uint8_t>);
    inline void DecodeAccumulatorSecondaryPointerMove(uint8_t, interfaces::DecodedInstruction&, std::optional<uint8_t>);
    inline void DecodeAccumulatorRegisterImplicitMove(interfaces::DecodedInstruction&, std::optional<uint8_t>);
    inline void DecodeRegisterImplicitAccumulatorMove(interfaces::DecodedInstruction&, std::optional<uint8_t>);
    inline void DecodeAccumulatorImmediateImplicitMove(interfaces::DecodedInstruction&, std::optional<uint8_t>);
    inline void DecodeImmediateImplicitAccumulatorMove(interfaces::DecodedInstruction&, std::optional<uint8_t>);
    inline void DecodeAccumulatorExtendedImmediateMove(interfaces::DecodedInstruction&, std::optional<uint8_t>);
    inline void DecodeExtendedImmediateAccumulatorMove(interfaces::DecodedInstruction&, std::optional<uint8_t>);
    inline void DecodeExtendedImmediatePairMove(interfaces::DecodedInstruction&, std::optional<uint8_t>);
    inline void DecodeAccumulatorPointerIncrementMove(interfaces::DecodedInstruction&, std::optional<uint8_t>);
    inline void DecodeAccumulatorPointerDecrementMove(interfaces::DecodedInstruction&, std::optional<uint8_t>);
    inline void DecodeSecondaryPointerAccumulatorMove(uint8_t, interfaces::DecodedInstruction&, std::optional<uint8_t>);
    inline void DecodePointerIncrementAccumulatorMove(interfaces::DecodedInstruction&, std::optional<uint8_t>);
    inline void DecodePointerDecrementAccumulatorMove(interfaces::DecodedInstruction&, std::optional<uint8_t>);
    inline void DecodeRegisterPairImmediateMove(uint8_t, interfaces::DecodedInstruction&, std::optional<uint8_t>);
    inline void DecodeStackPointerPointerMove(interfaces::DecodedInstruction&, std::optional<uint8_t>);

    inline void ExecuteRegisterMemoryOperand1Transfer(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    inline void ExecuteRegisterMemoryOperand2Transfer(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    inline void ExecuteRegisterMemoryOperand3Transfer(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    inline void ExecuteRegisterMemoryOperand1And2Transfer(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    inline void ExecuteRegisterRegisterTransfer(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    inline void ExecuteRegisterPairRegisterPairTransfer(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    inline void ExecuteRegisterMemoryTransfer(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    inline void ExecuteRegisterPairTransferToMemory(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    inline void ExecuteMemoryOperand1MemoryTransfer(interfaces::DecodedInstruction&);
    inline void DecodeRegisterIndexedMove(uint8_t, uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeIndexedRegisterMove(uint8_t, uint8_t, interfaces::DecodedInstruction&);

};

}