#pragma once

#include <cstdint>
#include <optional>
#include <memory>

#include "AddressingMode.h"
#include "AddressingModeFormat.h"
#include "MemoryController.h"
#include "Opcodes.h"
#include "MemoryControllerInterface.h"
#include "RegisterBankInterface.h"
#include "SystemMode.h"

namespace gbxcore::interfaces
{

struct DecodedInstruction
{
    instructions::OpcodeType Opcode;
    AddressingMode AddressingMode;
    uint8_t MemoryOperand1;
    uint8_t MemoryOperand2;
    uint8_t MemoryOperand3;
    Register SourceRegister;
    Register DestinationRegister;
    uint8_t MemoryResult1;
    uint8_t MemoryResult2;
    uint8_t InstructionExtraOperand;
};

class InstructionConstants
{
public:
    const static uint16_t PreOpcode_DD = 0xDD;
    const static uint16_t PreOpcode_CB = 0xCB;

    // Instructions Pre-opcode reserved for GBX/System Mode
    const static uint16_t PreOpcode_FD = 0xFD; // Indexed data loading 
    const static uint16_t PreOpcode_FC = 0xFC; // System Mode operartions 
};

typedef bool ArithmeticLogicUnitSignal;

class ArithmeticLogicUnitInterface
{
public:
    virtual ~ArithmeticLogicUnitInterface() = default;    
    
    virtual void Initialize(std::shared_ptr<RegisterBankInterface>) = 0;

    virtual void InitializeRegisters() = 0;
    virtual void Decode() = 0;
    virtual void Execute() = 0;
    
    virtual void AcquireInstruction(std::shared_ptr<interfaces::MemoryControllerInterface> memoryController) = 0;

    [[nodiscard]] virtual bool ClearInterruptStatusSignal() = 0;
    [[nodiscard]] virtual bool HaltSignal() = 0;
    [[nodiscard]] virtual bool StopSignal() = 0;
    [[nodiscard]] virtual bool InterruptMasterEnable() = 0;
    [[nodiscard]] virtual bool IsExecutionAborted() = 0;
    [[nodiscard]] virtual bool UserModeRequested() = 0; 
    [[nodiscard]] virtual bool UserModeSourceOperandRequested() = 0;

    virtual AddressingModeFormat* AcquireAddressingModeTraits() = 0;
    virtual void AcquireOperand1AtPC(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void AcquireOperand1Implicitly(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void AcquireOperand1AtRegister(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void AcquireOperand2AtPC(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void AcquireOperand2AtComposedAddress(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void AcquireOperand2Implicitly(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void AcquireOperand2Directly(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void AcquireOperand3(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void WriteBackAtOperandAddress(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void WriteBackAtRegisterAddress(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void WriteBackAtComposedAddress(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void WriteBackAtImplicitRegisterAddress(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void WriteBackAtImplicitImmediateAddress(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void WriteBackPairAtRegisterAddress(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void WriteBackPairAtImmediareAddress(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
};


}