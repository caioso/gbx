#pragma once

#include <memory>

#include "GBXExceptions.h"
#include "AddressingModeFormat.h"
#include "interfaces/InstructionInterface.h"
#include "instructions/InstructionUtilities.h"
#include "instructions/OpcodeDecoder.h"
#include "instructions/OpcodePatternMatcher.h"
#include "interfaces/ArithmeticLogicUnitInterface.h"
#include "interfaces/MemoryControllerInterface.h"

namespace gbx
{

class ArithmeticLogicUnit : public interfaces::ArithmeticLogicUnitInterface
{
public:
    ArithmeticLogicUnit();
    virtual ~ArithmeticLogicUnit() = default;
    virtual void Decode(uint8_t, std::optional<uint8_t>) override;
    virtual void Execute(std::shared_ptr<RegisterBank>) override;

    virtual AddressingModeFormat* AcquireAddressingModeTraits() override;
    virtual void AcquireOperand1AtPC(std::shared_ptr<RegisterBank>, std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void AcquireOperand1Implicitly(std::shared_ptr<RegisterBank>, std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void AcquireOperand1AtRegister(std::shared_ptr<RegisterBank>, std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void AcquireOperand2AtPC(std::shared_ptr<RegisterBank>, std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void AcquireOperand2AtComposedAddress(std::shared_ptr<RegisterBank>, std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void AcquireOperand2Implicitly(std::shared_ptr<RegisterBank>, std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void AcquireOperand3(std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void WriteBackAtOperandAddress(std::shared_ptr<RegisterBank>, std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void WriteBackAtRegisterAddress(std::shared_ptr<RegisterBank>, std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void WriteBackAtComposedAddress(std::shared_ptr<RegisterBank>, std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void WriteBackAtImplicitRegisterAddress(std::shared_ptr<RegisterBank>, std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void WriteBackAtImplicitImmediateAddress(std::shared_ptr<RegisterBank>, std::shared_ptr<interfaces::MemoryControllerInterface>) override;

protected:
    inline uint8_t ReadAtRegister(Register, std::shared_ptr<RegisterBank>, std::shared_ptr<interfaces::MemoryControllerInterface>);
    inline void IncrementRegisterPair(Register, std::shared_ptr<RegisterBank>);
    inline void DecrementRegisterPair(Register, std::shared_ptr<RegisterBank>);
    inline void IncrementPC(std::shared_ptr<RegisterBank>);

    interfaces::DecodedInstruction _instructionData;
    std::shared_ptr<interfaces::InstructionInterface> _currentInstruction;
    OpcodeDecoder _decoder;
    AddressingModeFormat* _currentAddressingMode;
};

}