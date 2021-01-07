#include "ControlUnit.h"

using namespace std;
using namespace gbx::interfaces;

namespace gbx
{

ControlUnit::ControlUnit()
    : _registers(make_shared<RegisterBank>())
    , _preOpcode(nullopt)
    , _currentAddressingMode(nullptr)
{
    InitializeRegisters();
}

void ControlUnit::Initialize(shared_ptr<MemoryControllerInterface> memoryController, shared_ptr<ArithmeticLogicUnitInterface> alu)
{
    _memoryController = memoryController;
    _alu = alu;
}

void ControlUnit::RunCycle()
{
    // 1 Fetch
    Fetch();

    // 1.1 Fetch Real Opcode
    if (_preOpcode != nullopt)
        FetchAgain();

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
    auto instruction = ReadAtRegister(Register::PC);
    IncrementPC();

    if (instruction == 0xDD || instruction == 0xFD)
        CompletePreOpcodeFetch(instruction);
    else
        CompleteFetchPC(instruction);
}

inline void ControlUnit::FetchAgain()
{
    auto instruction = ReadAtRegister(Register::PC);
    IncrementPC();
    CompleteFetchPC(instruction);
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
    _registers->Write(Register::IR, 0x00);
    _registers->WritePair(Register::PC, 0x0000);
    _registers->Write(Register::F, 0x00);
}

inline uint8_t ControlUnit::ReadAtRegister(Register reg)
{
    auto registerContent = _registers->ReadPair(reg);
    return get<uint8_t>(_memoryController->Read(registerContent, MemoryAccessType::Byte));
}

inline void ControlUnit::IncrementPC()
{
    auto programCounter = _registers->ReadPair(Register::PC);
    _registers->WritePair(Register::PC, programCounter + 1);
}

inline void ControlUnit::CompleteFetchPC(uint8_t instruction)
{
    _registers->Write(Register::IR, instruction);
}

inline void ControlUnit::CompletePreOpcodeFetch(uint8_t preOpcode)
{
    _preOpcode = preOpcode;
}

inline void ControlUnit::DecodeInstruction()
{
    auto opcode = _registers->Read(Register::IR);
    _alu->Decode(opcode, _preOpcode);
    _preOpcode = nullopt;
}

inline void ControlUnit::AcquireAddressingMode()
{
    _currentAddressingMode = _alu->AcquireAddressingModeTraits();
}

inline void ControlUnit::ReadOperand1AtPC()
{
    _alu->AcquireOperand1AtPC(_registers, _memoryController);
}

inline void ControlUnit::ReadOperand1AtRegister()
{
    _alu->AcquireOperand1AtRegister(_registers, _memoryController);
}

inline void ControlUnit::ReadOperand1Implicitly()
{
    _alu->AcquireOperand1Implicitly(_registers, _memoryController);
}

inline void ControlUnit::ReadOperand2AtPC()
{
    _alu->AcquireOperand2AtPC(_registers, _memoryController);
}

inline void ControlUnit::ReadOperand2AtComposedAddress()
{
    _alu->AcquireOperand2AtComposedAddress(_registers, _memoryController);
}

inline void ControlUnit::ReadOperand2Implicitly()
{
    _alu->AcquireOperand2Implicitly(_registers, _memoryController);
}

inline void ControlUnit::ExecuteInstruction()
{
    _alu->Execute(_registers);
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
}

inline void ControlUnit::WriteBackAtOperandAddress()
{
    _alu->WriteBackAtOperandAddress(_registers, _memoryController);
}

inline void ControlUnit::WriteBackAtRegisterAddress()
{
    _alu->WriteBackAtRegisterAddress(_registers, _memoryController);
}

inline void ControlUnit::WriteBackAtComposedAddress()
{
    _alu->WriteBackAtComposedAddress(_registers, _memoryController);
}

inline void ControlUnit::WriteBackAtImplicitRegisterAddress()
{
    _alu->WriteBackAtImplicitRegisterAddress(_registers, _memoryController);
}

inline void ControlUnit::WriteBackAtImplicitImmediateAddress()
{
    _alu->WriteBackAtImplicitImmediateAddress(_registers, _memoryController);
}

}