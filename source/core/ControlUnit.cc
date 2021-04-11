#include "ControlUnit.h"

using namespace std;
using namespace gbxcore::interfaces;

namespace gbxcore
{

ControlUnit::ControlUnit()
    : _preOpcode(nullopt)
    , _currentAddressingMode(nullptr)
{}

void ControlUnit::Initialize(MemoryControllerInterface* memoryController, ArithmeticLogicUnitInterface* alu)
{
    _memoryController = memoryController;
    _alu = alu;

    call_once(_flag, [&]() -> void { _alu->InitializeRegisters(); });
}

void ControlUnit::RunCycle()
{
    // 1 Fetch
    Fetch();

    // 2 Decode Instruction
    Decode();

    // 2.1 Acquire Operand 1 or displacement
    if (_currentAddressingMode->acquireOperand1)
        AcquireOperand1();
    
    // 2.2 Acquire Operand 2 or Operand from address + displacement or MSByte of 
    if (_currentAddressingMode->acquireOperand2)
        AcquireOperand2();

    // 2.3 Acquire Operand 3 (extended source) 
    if (_currentAddressingMode->acquireOperand3)
        AcquireOperand3();

    // 3 Execute
    Execute();

    // 4 WriteBack
    if (_currentAddressingMode->writeBack && !IsExecutionAborted())
        WriteBack();
}

inline void ControlUnit::Fetch()
{
    _alu->AcquireInstruction(_memoryController);
}

inline void ControlUnit::Decode()
{
    DecodeInstruction();
    AcquireAddressingMode();
}

inline void ControlUnit::AcquireOperand1()
{
    if (_currentAddressingMode->acquireOperand1FromPc)
        ReadOperand1AtPC();
    else
    {
        auto oldMode = Mode::System;
        if (IsUserSourceOperandModeRequested())
        {
            oldMode = _memoryController->Mode();
            _memoryController->SetMode(Mode::User);            
        }

        if (_currentAddressingMode->acquireOperand1Directly)
            ReadOperand1AtRegister();
        else if (_currentAddressingMode->acquireOperand1Implicitly)
            ReadOperand1Implicitly();

        if (IsUserSourceOperandModeRequested())
            _memoryController->SetMode(oldMode);            
    }
}

inline void ControlUnit::AcquireOperand2()
{
    if (_currentAddressingMode->acquireOperand2FromPc)
        ReadOperand2AtPC();
    else
    { 
        auto oldMode = Mode::System;
        if (IsUserSourceOperandModeRequested())
        {
            oldMode = _memoryController->Mode();
            _memoryController->SetMode(Mode::User);            
        }

        if (IsUserSourceOperandModeRequested())
            _memoryController->SetMode(Mode::User);            

        if (_currentAddressingMode->acquireOperand2AtComposedAddress)
            ReadOperand2AtComposedAddress();
        else if (_currentAddressingMode->acquireOperand2Implicitly)
            ReadOperand2Implicitly();
        else if (_currentAddressingMode->acquireOperand2Directly)
            ReadOperand2Directly();

        if (IsUserSourceOperandModeRequested())
            _memoryController->SetMode(oldMode);            
    }
}

inline void ControlUnit::AcquireOperand3()
{
    auto oldMode = Mode::System;
    if (IsUserSourceOperandModeRequested())
    {
        oldMode = _memoryController->Mode();
        _memoryController->SetMode(Mode::User);            
    }

    _alu->AcquireOperand3(_memoryController);

    if (IsUserSourceOperandModeRequested())
        _memoryController->SetMode(oldMode);            
}

inline void ControlUnit::Execute()
{
    ExecuteInstruction();
}

inline void ControlUnit::WriteBack()
{
    WriteBackResults();
}

inline void ControlUnit::InitializeRegisters()
{
    _alu->InitializeRegisters();
}

inline void ControlUnit::DecodeInstruction()
{
    _alu->Decode();
}

inline void ControlUnit::AcquireAddressingMode()
{
    _currentAddressingMode = _alu->AcquireAddressingModeTraits();
}

inline void ControlUnit::ReadOperand1AtPC()
{
    _alu->AcquireOperand1AtPC(_memoryController);
}

inline void ControlUnit::ReadOperand1AtRegister()
{
    _alu->AcquireOperand1AtRegister(_memoryController);
}

inline void ControlUnit::ReadOperand1Implicitly()
{
    _alu->AcquireOperand1Implicitly(_memoryController);
}

inline void ControlUnit::ReadOperand2AtPC()
{
    _alu->AcquireOperand2AtPC(_memoryController);
}

inline void ControlUnit::ReadOperand2AtComposedAddress()
{
    _alu->AcquireOperand2AtComposedAddress(_memoryController);
}

inline void ControlUnit::ReadOperand2Implicitly()
{
    _alu->AcquireOperand2Implicitly(_memoryController);
}

inline void ControlUnit::ReadOperand2Directly()
{
    _alu->AcquireOperand2Directly(_memoryController);
}

inline void ControlUnit::ExecuteInstruction()
{
    _alu->Execute();

    if (IsUserModeRequested())
        _memoryController->SetMode(Mode::User);
}

inline void ControlUnit::WriteBackResults()
{
    if (_currentAddressingMode->writeBackAtOperandAddress)
        WriteBackAtOperandAddress();
    else if (_currentAddressingMode->writeBackAtRegisterAddress)
        WriteBackAtRegisterAddress();
    else if (_currentAddressingMode->writeBackAtComposedOperandAddress)
        WriteBackAtComposedAddress();
    else if (_currentAddressingMode->writeBackAtImplicitlyWithRegister)
        WriteBackAtImplicitRegisterAddress();
    else if (_currentAddressingMode->writeBackAtImplicitlyWithImmediateOperand)
        WriteBackAtImplicitImmediateAddress();
    else if (_currentAddressingMode->writeBackPairAtRegisterAddress)
        WriteBackPairAtRegisterAddress();
    else if (_currentAddressingMode->writeBackPairAtImmediateAddress)
        WriteBackPairAtImmediateAddress();
}

inline void ControlUnit::WriteBackAtOperandAddress()
{
    _alu->WriteBackAtOperandAddress(_memoryController);
}

inline void ControlUnit::WriteBackAtRegisterAddress()
{
    _alu->WriteBackAtRegisterAddress(_memoryController);
}

inline void ControlUnit::WriteBackAtComposedAddress()
{
    _alu->WriteBackAtComposedAddress(_memoryController);
}

inline void ControlUnit::WriteBackAtImplicitRegisterAddress()
{
    _alu->WriteBackAtImplicitRegisterAddress(_memoryController);
}

inline void ControlUnit::WriteBackAtImplicitImmediateAddress()
{
    _alu->WriteBackAtImplicitImmediateAddress(_memoryController);
}

inline void ControlUnit::WriteBackPairAtRegisterAddress()
{
    _alu->WriteBackPairAtRegisterAddress(_memoryController);
}

inline void ControlUnit::WriteBackPairAtImmediateAddress()
{
    _alu->WriteBackPairAtImmediareAddress(_memoryController);
}

inline bool ControlUnit::IsExecutionAborted()
{
    return _alu->IsExecutionAborted();
}

inline bool ControlUnit::IsUserModeRequested()
{
    return _alu->UserModeRequested();
}

inline bool ControlUnit::IsUserSourceOperandModeRequested()
{
    return _alu->UserModeSourceOperandRequested();
}

}