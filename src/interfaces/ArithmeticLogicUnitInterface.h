#pragma once

#include <cstdint>
#include <optional>
#include <memory>

#include "../AddressingMode.h"
#include "../AddressingModeFormat.h"
#include "../MemoryController.h"
#include "../instructions/Opcodes.h"
#include "MemoryControllerInterface.h"
#include "RegisterBankInterface.h"

namespace gbx::interfaces
{

struct DecodedInstruction
{
    OpcodeType Opcode;
    AddressingMode AddressingMode;
    uint8_t MemoryOperand1;
    uint8_t MemoryOperand2;
    uint8_t MemoryOperand3;
    Register SourceRegister;
    Register DestinationRegister;
    uint8_t MemoryResult1;
};
class InstructionConstants
{
public:
    const static uint16_t PreOpcode_DD = 0xDD;
    const static uint16_t PreOpcode_FD = 0xFD;
};

class ArithmeticLogicUnitInterface
{
public:
    virtual ~ArithmeticLogicUnitInterface() = default;    
    
    virtual void Initialize(std::shared_ptr<RegisterBankInterface>) = 0;

    virtual void InitializeRegisters() = 0;
    virtual void Decode() = 0;
    virtual void Execute() = 0;
    
    virtual uint8_t AcquireInstruction(std::shared_ptr<interfaces::MemoryControllerInterface> memoryController) = 0;

    virtual AddressingModeFormat* AcquireAddressingModeTraits() = 0;
    virtual void AcquireOperand1AtPC(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void AcquireOperand1Implicitly(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void AcquireOperand1AtRegister(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void AcquireOperand2AtPC(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void AcquireOperand2AtComposedAddress(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void AcquireOperand2Implicitly(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void AcquireOperand3(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void WriteBackAtOperandAddress(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void WriteBackAtRegisterAddress(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void WriteBackAtComposedAddress(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void WriteBackAtImplicitRegisterAddress(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
    virtual void WriteBackAtImplicitImmediateAddress(std::shared_ptr<interfaces::MemoryControllerInterface>) = 0;
};


}