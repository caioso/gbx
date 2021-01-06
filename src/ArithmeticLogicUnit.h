#pragma once

#include <memory>

#include <iostream>

#include "GBXExceptions.h"
#include "instructions/InstructionUtilities.h"
#include "instructions/OpcodePatternMatcher.h"
#include "interfaces/ArithmeticLogicUnitInterface.h"

namespace gbx
{

class ArithmeticLogicUnit : public interfaces::ArithmeticLogicUnitInterface
{
public:
    ArithmeticLogicUnit() = default;
    virtual ~ArithmeticLogicUnit() = default;
    virtual void Decode(uint8_t, std::optional<uint8_t> preOpcode) override;
    virtual void Execute(std::shared_ptr<RegisterBank>) override;

private:
    inline void DecodeImmediateOperand(uint8_t);
    inline void DecodeRegisterIndirectOperandSourceBCDE(uint8_t);
    inline void DecodeRegisterIndirectOperandSourceHL(uint8_t);
    inline void DecodeRegisterIndirectOperandDestinationHL(uint8_t);
    inline void DecodeRegisterRegisterOperand(uint8_t);
    inline void DecodeRegisterPairRegisterPairOperand();
    inline void DecodeRegisterIndirectOperandDestinationBCDE(uint8_t);
    inline void DecodeRegisterIndexedSource(uint8_t, uint8_t);
    inline void DecodeRegisterIndexedDestination(uint8_t, uint8_t);
    inline void DecodeExtendedOperand(uint8_t);
    inline void DecodeImmediateRegisterIndirect();
    inline void DecodeRegisterIndirectSourceIncrementDecrement(uint8_t);
    inline void DecodeRegisterIndirectDestinationIncrementDecrement(uint8_t);
    inline void DecodeRegisterImplicitOperand(uint8_t);
    inline void DecodeImmediateImplicitOperand(uint8_t);
    inline void DecodeRegisterImmediatePair(uint8_t);
    inline void DecodeAddRegisterMode(uint8_t);
    inline void DecodeAddImmediateMode();

    inline void ExecuteMemoryBasedSource(std::shared_ptr<RegisterBank>);
    inline void ExecuteTwoOperandsMemoryBaseSource(std::shared_ptr<RegisterBank>);
    inline void ExecuteMemoryBasedDestination(std::shared_ptr<RegisterBank>);
    inline void ExecuteRegisterSourceOrDestination(std::shared_ptr<RegisterBank>);
    inline void ExecuteExtendedSource(std::shared_ptr<RegisterBank>);
    inline void ExecuteImediateRegisterIndirect();
    inline void ExecuteMemoryBaseSourceOnPair(std::shared_ptr<RegisterBank>);
    inline void ExecuteRegisterPairAddressingMode(std::shared_ptr<RegisterBank>);
    inline void ExecuteAdd(std::shared_ptr<RegisterBank>);
};

}