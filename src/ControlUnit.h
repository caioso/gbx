#pragma once

#include <memory>
#include <optional>
#include <variant>
#include <sstream>

#include <iostream>

#include "AddressingModeFormat.h"
#include "GBXCoreExceptions.h"
#include "RegisterBank.h"
#include "ArithmeticLogicUnit.h"
#include "interfaces/ArithmeticLogicUnitInterface.h"
#include "interfaces/ControlUnitInterface.h"
#include "interfaces/MemoryControllerInterface.h"

namespace gbx
{
class ControlUnit : public std::enable_shared_from_this<ControlUnit>, public interfaces::ControlUnitInterface
{
public:
    ControlUnit();
    virtual ~ControlUnit() = default;
    virtual void RunCycle() override;

    virtual void Initialize(std::shared_ptr<interfaces::MemoryControllerInterface>,
                            std::shared_ptr<interfaces::ArithmeticLogicUnitInterface>) override;

protected:
    inline void Fetch();
    inline void Decode();
    inline void Execute();
    inline void AcquireOperand1();
    inline void AcquireOperand2();
    inline void AcquireOperand3();
    inline void WriteBack();

    inline void AcquireAddressingMode();
    inline void InitializeRegisters();
    inline void DecodeInstruction();
    inline void ExecuteInstruction();
    inline void WriteBackResults();
    inline void ReadOperand1AtPC();
    inline void ReadOperand1AtRegister();
    inline void ReadOperand1Implicitly();
    inline void ReadOperand2AtPC();
    inline void ReadOperand2AtComposedAddress();
    inline void ReadOperand2Implicitly();
    inline void ReadOperand2Directly();
    inline void IncrementRegisterPair(interfaces::Register);
    inline void DecrementRegisterPair(interfaces::Register);
    inline void WriteBackAtOperandAddress();
    inline void WriteBackAtRegisterAddress();
    inline void WriteBackPairAtRegisterAddress(); 
    inline void WriteBackAtComposedAddress();
    inline void WriteBackAtImplicitRegisterAddress();
    inline void WriteBackAtImplicitImmediateAddress();
    inline void WriteBackPairAtImmediateAddress();

    inline bool IsExecutionAborted();

    std::shared_ptr<interfaces::ArithmeticLogicUnitInterface> _alu;
    std::optional<uint8_t> _preOpcode;
    std::shared_ptr<interfaces::MemoryControllerInterface> _memoryController;
    AddressingModeFormat* _currentAddressingMode;   

    std::once_flag _flag;
};

}