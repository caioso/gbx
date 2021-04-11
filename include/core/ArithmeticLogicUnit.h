#pragma once

#include <memory>

#include "GBXCoreExceptions.h"
#include "AddressingModeFormat.h"
#include "InstructionUtilities.h"
#include "OpcodeDecoder.h"
#include "OpcodePatternMatcher.h"
#include "ArithmeticLogicUnitInterface.h"
#include "InstructionInterface.h"
#include "MemoryControllerInterface.h"
#include "RegisterBankInterface.h"

namespace gbxcore
{

class ArithmeticLogicUnit : public interfaces::ArithmeticLogicUnitInterface
{
public:
    explicit ArithmeticLogicUnit();
    virtual ~ArithmeticLogicUnit() = default;

    virtual void Initialize(interfaces::RegisterBankInterface*) override;
    virtual void InitializeRegisters() override;

    virtual void Decode() override;
    virtual void Execute() override;

    virtual void AcquireInstruction(interfaces::MemoryControllerInterface* memoryController) override;

    [[nodiscard]] virtual bool ClearInterruptStatusSignal() override;
    [[nodiscard]] virtual bool HaltSignal() override;
    [[nodiscard]] virtual bool StopSignal() override;
    [[nodiscard]] virtual bool InterruptMasterEnable() override;
    [[nodiscard]] virtual bool IsExecutionAborted() override;
    [[nodiscard]] virtual bool UserModeRequested() override;
    [[nodiscard]] virtual bool UserModeSourceOperandRequested() override;

    virtual AddressingModeFormat* AcquireAddressingModeTraits() override;
    virtual void AcquireOperand1AtPC(interfaces::MemoryControllerInterface*) override;
    virtual void AcquireOperand1Implicitly(interfaces::MemoryControllerInterface*) override;
    virtual void AcquireOperand1AtRegister(interfaces::MemoryControllerInterface*) override;
    virtual void AcquireOperand2AtPC(interfaces::MemoryControllerInterface*) override;
    virtual void AcquireOperand2AtComposedAddress(interfaces::MemoryControllerInterface*) override;
    virtual void AcquireOperand2Implicitly(interfaces::MemoryControllerInterface*) override;
    virtual void AcquireOperand2Directly(interfaces::MemoryControllerInterface*) override;
    virtual void AcquireOperand3(interfaces::MemoryControllerInterface*) override;
    virtual void WriteBackAtOperandAddress(interfaces::MemoryControllerInterface*) override;
    virtual void WriteBackAtRegisterAddress(interfaces::MemoryControllerInterface*) override;
    virtual void WriteBackAtComposedAddress(interfaces::MemoryControllerInterface*) override;
    virtual void WriteBackAtImplicitRegisterAddress(interfaces::MemoryControllerInterface*) override;
    virtual void WriteBackAtImplicitImmediateAddress(interfaces::MemoryControllerInterface*) override;
    virtual void WriteBackPairAtRegisterAddress(interfaces::MemoryControllerInterface*) override;
    virtual void WriteBackPairAtImmediareAddress(interfaces::MemoryControllerInterface*) override;

protected:
    inline uint8_t ReadAtRegister(interfaces::Register, interfaces::MemoryControllerInterface*);
    inline void IncrementRegisterPair(interfaces::Register);
    inline void DecrementRegisterPair(interfaces::Register);
    inline void IncrementPC();
    inline static bool IsSuffixedInstruction(uint8_t);
    inline void ResolveExecutionSignals();
    inline void ResolveMemoryAccessSignals();
    inline void ClearExecutionSignals();

    AddressingModeFormat* _currentAddressingMode;
    interfaces::DecodedInstruction _instructionData;
    instructions::OpcodeDecoder _decoder;
    interfaces::BaseInstructionInterface* _currentInstruction{};
    interfaces::RegisterBankInterface* _registers;
    
    bool _userModeRequested{};
    bool _userModeSourceOperandRequested{};
    bool _executionAborted{};
    bool _clearInterruptStatusSignal{};
    bool _haltSignal{};
    bool _stopSignal{};
    bool _interruptMasterEnable{};
};

}