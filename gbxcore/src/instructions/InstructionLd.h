#pragma once

#include "InstructionUtilities.h"
#include "../interfaces/InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionLd : public interfaces::InstructionInterface
{
public:
    InstructionLd() = default;
    virtual ~InstructionLd() = default;
    
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) override;
    virtual void Execute(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&) override;

private:
    inline void DecodeRegisterRegisterMove(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeRegisterImmediateMove(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeRegisterPointerMove(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodePointerRegisterMove(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodePointerImmediateMove(interfaces::DecodedInstruction&);
    inline void DecodeAccumulatorSecondaryPointerMove(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeAccumulatorRegisterImplicitMove(interfaces::DecodedInstruction&);
    inline void DecodeRegisterImplicitAccumulatorMove(interfaces::DecodedInstruction&);
    inline void DecodeAccumulatorImmediateImplicitMove(interfaces::DecodedInstruction&);
    inline void DecodeImmediateImplicitAccumulatorMove(interfaces::DecodedInstruction&);
    inline void DecodeAccumulatorExtendedImmediateMove(interfaces::DecodedInstruction&);
    inline void DecodeExtendedImmediateAccumulatorMove(interfaces::DecodedInstruction&);
    inline void DecodeExtendedImmediatePairMove(interfaces::DecodedInstruction&);
    inline void DecodeAccumulatorPointerIncrementMove(interfaces::DecodedInstruction&);
    inline void DecodeAccumulatorPointerDecrementMove(interfaces::DecodedInstruction&);
    inline void DecodeSecondaryPointerAccumulatorMove(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodePointerIncrementAccumulatorMove(interfaces::DecodedInstruction&);
    inline void DecodePointerDecrementAccumulatorMove(interfaces::DecodedInstruction&);
    inline void DecodeRegisterPairImmediateMove(uint8_t, interfaces::DecodedInstruction&);
    inline void DecodeStackPointerPointerMove(interfaces::DecodedInstruction&);

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