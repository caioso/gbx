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
#include "ArithmeticLogicUnitInterface.h"
#include "ControlUnitInterface.h"
#include "MemoryControllerInterface.h"

namespace gbxcore
{
class ControlUnit : public std::enable_shared_from_this<ControlUnit>, public interfaces::ControlUnitInterface
{
public:
    ControlUnit();
    virtual ~ControlUnit() = default;
    virtual void RunCycle() override;

    virtual void Initialize(interfaces::MemoryControllerInterface*,
                            interfaces::ArithmeticLogicUnitInterface*) override;

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
    inline bool IsUserModeRequested();
    inline bool IsUserSourceOperandModeRequested();

    interfaces::ArithmeticLogicUnitInterface*  _alu;
    interfaces::MemoryControllerInterface* _memoryController;
    std::optional<uint8_t> _preOpcode;
    AddressingModeFormat* _currentAddressingMode;   

    std::once_flag _flag;
};

}