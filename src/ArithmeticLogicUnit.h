#pragma once

#include <memory>

#include "GBXExceptions.h"
#include "AddressingModeFormat.h"
#include "instructions/InstructionUtilities.h"
#include "instructions/OpcodeDecoder.h"
#include "instructions/OpcodePatternMatcher.h"
#include "interfaces/ArithmeticLogicUnitInterface.h"
#include "interfaces/InstructionInterface.h"
#include "interfaces/MemoryControllerInterface.h"
#include "interfaces/RegisterBankInterface.h"

namespace gbx
{

class ArithmeticLogicUnit : public interfaces::ArithmeticLogicUnitInterface
{
public:
    ArithmeticLogicUnit();
    virtual ~ArithmeticLogicUnit() = default;

    virtual void Initialize(std::shared_ptr<interfaces::RegisterBankInterface>) override;
    virtual void InitializeRegisters() override;

    virtual void Decode() override;
    virtual void Execute() override;

    virtual uint8_t AcquireInstruction(std::shared_ptr<interfaces::MemoryControllerInterface> memoryController) override;

    virtual AddressingModeFormat* AcquireAddressingModeTraits() override;
    virtual void AcquireOperand1AtPC(std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void AcquireOperand1Implicitly(std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void AcquireOperand1AtRegister(std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void AcquireOperand2AtPC(std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void AcquireOperand2AtComposedAddress(std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void AcquireOperand2Implicitly(std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void AcquireOperand2Directly(std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void AcquireOperand3(std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void WriteBackAtOperandAddress(std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void WriteBackAtRegisterAddress(std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void WriteBackAtComposedAddress(std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void WriteBackAtImplicitRegisterAddress(std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void WriteBackAtImplicitImmediateAddress(std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void WriteBackPairAtRegisterAddress(std::shared_ptr<interfaces::MemoryControllerInterface>) override;
    virtual void WriteBackPairAtImmediareAddress(std::shared_ptr<interfaces::MemoryControllerInterface>) override;

protected:
    inline uint8_t ReadAtRegister(interfaces::Register, std::shared_ptr<interfaces::MemoryControllerInterface>);
    inline void IncrementRegisterPair(interfaces::Register);
    inline void DecrementRegisterPair(interfaces::Register);
    inline void IncrementPC();
    inline bool IsComplementedInstruction(uint8_t);
    inline bool IsSuffixedInstruction(uint8_t);

    interfaces::DecodedInstruction _instructionData;
    std::shared_ptr<interfaces::InstructionInterface> _currentInstruction;
    std::shared_ptr<interfaces::RegisterBankInterface> _registers;
    OpcodeDecoder _decoder;
    AddressingModeFormat* _currentAddressingMode;

private:
    const uint8_t RlcPreOpcode = 0xCB;
};

}