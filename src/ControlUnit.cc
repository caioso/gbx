#include "ControlUnit.h"

using namespace std;
using namespace gbx::interfaces;

namespace gbx
{

ControlUnit::ControlUnit()
    : _preOpcode(nullopt)
    , _currentAddressingMode(nullptr)
{}

void ControlUnit::Initialize(shared_ptr<MemoryControllerInterface> memoryController, shared_ptr<ArithmeticLogicUnitInterface> alu)
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
    if (_currentAddressingMode->writeBack)
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
    else if (_currentAddressingMode->acquireOperand1Directly)
        ReadOperand1AtRegister();
    else if (_currentAddressingMode->acquireOperand1Implicitly)
        ReadOperand1Implicitly();
}

inline void ControlUnit::AcquireOperand2()
{
    if (_currentAddressingMode->acquireOperand2FromPc)
        ReadOperand2AtPC();
    else if (_currentAddressingMode->acquireOperand2AtComposedAddress)
        ReadOperand2AtComposedAddress();
    else if (_currentAddressingMode->acquireOperand2Implicitly)
        ReadOperand2Implicitly();
    else if (_currentAddressingMode->acquireOperand2Directly)
        ReadOperand2Directly();
}

inline void ControlUnit::AcquireOperand3()
{
    _alu->AcquireOperand3(_memoryController);
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

}