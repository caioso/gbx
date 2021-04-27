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

    // Instructions Pre-opcode reserved for GBX/System SecurityLevel
    const static uint16_t PreOpcode_FD = 0xFD; // Indexed data loading 
    const static uint16_t PreOpcode_FC = 0xFC; // System SecurityLevel operartions 
};

typedef bool ArithmeticLogicUnitSignal;

class ArithmeticLogicUnitInterface
{
public:
    virtual ~ArithmeticLogicUnitInterface() = default;    
    
    virtual void Initialize(interfaces::RegisterBankInterface*) = 0;

    virtual void InitializeRegisters() = 0;
    virtual void Decode() = 0;
    virtual void Execute() = 0;
    
    virtual void AcquireInstruction(interfaces::MemoryControllerInterface* memoryController) = 0;

    [[nodiscard]] virtual bool ClearInterruptStatusSignal() = 0;
    [[nodiscard]] virtual bool HaltSignal() = 0;
    [[nodiscard]] virtual bool StopSignal() = 0;
    [[nodiscard]] virtual bool InterruptMasterEnable() = 0;
    [[nodiscard]] virtual bool IsExecutionAborted() = 0;
    [[nodiscard]] virtual bool UserModeRequested() = 0; 
    [[nodiscard]] virtual bool UserModeSourceOperandRequested() = 0;

    virtual AddressingModeFormat* AcquireAddressingModeTraits() = 0;
    virtual void AcquireOperand1AtPC(interfaces::MemoryControllerInterface*) = 0;
    virtual void AcquireOperand1Implicitly(interfaces::MemoryControllerInterface*) = 0;
    virtual void AcquireOperand1AtRegister(interfaces::MemoryControllerInterface*) = 0;
    virtual void AcquireOperand2AtPC(interfaces::MemoryControllerInterface*) = 0;
    virtual void AcquireOperand2AtComposedAddress(interfaces::MemoryControllerInterface*) = 0;
    virtual void AcquireOperand2Implicitly(interfaces::MemoryControllerInterface*) = 0;
    virtual void AcquireOperand2Directly(interfaces::MemoryControllerInterface*) = 0;
    virtual void AcquireOperand3(interfaces::MemoryControllerInterface*) = 0;
    virtual void WriteBackAtOperandAddress(interfaces::MemoryControllerInterface*) = 0;
    virtual void WriteBackAtRegisterAddress(interfaces::MemoryControllerInterface*) = 0;
    virtual void WriteBackAtComposedAddress(interfaces::MemoryControllerInterface*) = 0;
    virtual void WriteBackAtImplicitRegisterAddress(interfaces::MemoryControllerInterface*) = 0;
    virtual void WriteBackAtImplicitImmediateAddress(interfaces::MemoryControllerInterface*) = 0;
    virtual void WriteBackPairAtRegisterAddress(interfaces::MemoryControllerInterface*) = 0;
    virtual void WriteBackPairAtImmediareAddress(interfaces::MemoryControllerInterface*) = 0;
};


}