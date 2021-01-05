#include "ControlUnit.h"

using namespace std;
using namespace gbx::interfaces;

namespace gbx
{

ControlUnit::ControlUnit()
    : _registers(make_shared<RegisterBank>())
    , _preOpcode(nullopt)
    , _currentAddressingMode(make_shared<AddressingModeFormat>(AddressingModeTemplate::NoAddressingMode))
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
    auto operandLocation = static_cast<uint16_t>(_alu->InstructionData.value().MemoryOperand1 | _alu->InstructionData.value().MemoryOperand2 << 8);
    _alu->InstructionData.value().MemoryOperand3 = get<uint8_t>(_memoryController->Read(operandLocation, MemoryAccessType::Byte));
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
    InstantiateInstruction(opcode);
    _preOpcode = nullopt;
}

inline void ControlUnit::InstantiateInstruction(uint8_t opcode)
{
    _alu->Decode(opcode, _preOpcode);
}

inline void ControlUnit::AcquireAddressingMode()
{
    switch (_alu->InstructionData.value().AddressingMode)
    {
        case AddressingMode::Register: _currentAddressingMode = make_shared<AddressingModeFormat>(AddressingModeTemplate::RegisterAddressingMode); break;
        case AddressingMode::Immediate: _currentAddressingMode = make_shared<AddressingModeFormat>(AddressingModeTemplate::ImmediateAddressingMode); break;
        case AddressingMode::RegisterIndexedSource: _currentAddressingMode = make_shared<AddressingModeFormat>(AddressingModeTemplate::RegisterIndexedSourceAddressingMode); break;
        case AddressingMode::RegisterIndexedDestination: _currentAddressingMode = make_shared<AddressingModeFormat>(AddressingModeTemplate::RegisterIndexedDestinationAddressingMode); break;
        case AddressingMode::RegisterIndirectSource: _currentAddressingMode = make_shared<AddressingModeFormat>(AddressingModeTemplate::RegisterIndirectSourceAddressingMode); break;
        case AddressingMode::RegisterIndirectDestination: _currentAddressingMode = make_shared<AddressingModeFormat>(AddressingModeTemplate::RegisterIndirectDestinationAddressingMode); break;
        case AddressingMode::ExtendedSource: _currentAddressingMode = make_shared<AddressingModeFormat>(AddressingModeTemplate::RegisterExtendedSourceAddressingMode); break;
        case AddressingMode::ExtendedDestination: _currentAddressingMode = make_shared<AddressingModeFormat>(AddressingModeTemplate::RegisterExtendedDestinationAddressingMode); break;
        case AddressingMode::ImmediateRegisterIndirect: _currentAddressingMode = make_shared<AddressingModeFormat>(AddressingModeTemplate::ImmediateRegisterIndirectAddressingMode); break;
        case AddressingMode::RegisterIndirectSourceIncrement: _currentAddressingMode = make_shared<AddressingModeFormat>(AddressingModeTemplate::ImmediateRegisterIndirectSourceIncrementAddressingMode); break;
        case AddressingMode::RegisterIndirectSourceDecrement: _currentAddressingMode = make_shared<AddressingModeFormat>(AddressingModeTemplate::ImmediateRegisterIndirectSourceDecrementAddressingMode); break;
        case AddressingMode::RegisterIndirectDestinationIncrement: _currentAddressingMode = make_shared<AddressingModeFormat>(AddressingModeTemplate::ImmediateRegisterIndirectDestinationIncrementAddressingMode); break;
        case AddressingMode::RegisterIndirectDestinationDecrement: _currentAddressingMode = make_shared<AddressingModeFormat>(AddressingModeTemplate::ImmediateRegisterIndirectDestinationDecrementAddressingMode); break;
        case AddressingMode::RegisterImplicitSource: _currentAddressingMode = make_shared<AddressingModeFormat>(AddressingModeTemplate::ImplicitRegisterSourceAddressingMode); break;
        case AddressingMode::RegisterImplicitDestination: _currentAddressingMode = make_shared<AddressingModeFormat>(AddressingModeTemplate::ImplicitRegisterDestinationAddressingMode); break;
        case AddressingMode::ImmediateImplicitSource: _currentAddressingMode = make_shared<AddressingModeFormat>(AddressingModeTemplate::ImplicitImmediateSourceAddressingMode); break;
        case AddressingMode::ImmediateImplicitDestination: _currentAddressingMode = make_shared<AddressingModeFormat>(AddressingModeTemplate::ImplicitImmediateDestinationAddressingMode); break;
        case AddressingMode::ImmediatePair: _currentAddressingMode = make_shared<AddressingModeFormat>(AddressingModeTemplate::ImmediatePairAddressingMode); break;
        case AddressingMode::RegisterPair: _currentAddressingMode = make_shared<AddressingModeFormat>(AddressingModeTemplate::RegisterPairAddressingMode); break;
        default:
            throw ArithmeticLogicUnitException("invalid addressing mode");
    }
}

inline void ControlUnit::ReadOperand1AtPC()
{
    _alu->InstructionData.value().MemoryOperand1 = ReadAtRegister(Register::PC);
    IncrementPC();
}

inline void ControlUnit::ReadOperand1AtRegister()
{
    _alu->InstructionData.value().MemoryOperand1 = ReadAtRegister(_alu->InstructionData.value().SourceRegister);

    if (_currentAddressingMode->incrementSource)
        IncrementRegisterPair(_alu->InstructionData.value().SourceRegister);
    else if (_currentAddressingMode->decrementSource)
        DecrementRegisterPair(_alu->InstructionData.value().SourceRegister);
}

inline void ControlUnit::ReadOperand1Implicitly()
{
    auto operandLocation = static_cast<uint16_t>(0xFF << 8 | _registers->Read(_alu->InstructionData.value().SourceRegister));
    _alu->InstructionData.value().MemoryOperand1 = get<uint8_t>(_memoryController->Read(operandLocation, MemoryAccessType::Byte));
}

inline void ControlUnit::ReadOperand2AtPC()
{
    _alu->InstructionData.value().MemoryOperand2 = ReadAtRegister(Register::PC);
    IncrementPC();
}

inline void ControlUnit::ReadOperand2AtComposedAddress()
{
    auto operandLocation = static_cast<uint16_t>(static_cast<int8_t>(_alu->InstructionData.value().MemoryOperand1) + _registers->ReadPair(_alu->InstructionData.value().SourceRegister));
    _alu->InstructionData.value().MemoryOperand2 = get<uint8_t>(_memoryController->Read(operandLocation, MemoryAccessType::Byte));
}

inline void ControlUnit::ReadOperand2Implicitly()
{
    auto operandLocation = static_cast<uint16_t>(0xFF << 8 | _alu->InstructionData.value().MemoryOperand1);
    _alu->InstructionData.value().MemoryOperand2 = get<uint8_t>(_memoryController->Read(operandLocation, MemoryAccessType::Byte));
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
    auto resultContent = _alu->InstructionData.value().MemoryResult1;
    auto resultAddress = static_cast<uint16_t>(static_cast<int8_t>(_alu->InstructionData.value().MemoryOperand1) + _registers->ReadPair(_alu->InstructionData.value().DestinationRegister));
    _memoryController->Write(static_cast<uint8_t>(resultContent), resultAddress);
}

inline void ControlUnit::WriteBackAtRegisterAddress()
{
    auto resultContent = _alu->InstructionData.value().MemoryResult1;
    auto resultAddress = _registers->ReadPair(_alu->InstructionData.value().DestinationRegister);
    _memoryController->Write(static_cast<uint8_t>(resultContent), resultAddress);

     if (_currentAddressingMode->incrementDestination)
        IncrementRegisterPair(_alu->InstructionData.value().DestinationRegister);
    else if (_currentAddressingMode->decrementDestination)
        DecrementRegisterPair(_alu->InstructionData.value().DestinationRegister);
}

inline void ControlUnit::WriteBackAtComposedAddress()
{
    auto resultContent = _alu->InstructionData.value().MemoryResult1;
    auto resultAddress = static_cast<uint16_t>(_alu->InstructionData.value().MemoryOperand1 | _alu->InstructionData.value().MemoryOperand2 << 8);
    _memoryController->Write(static_cast<uint8_t>(resultContent), resultAddress);
}

inline void ControlUnit::WriteBackAtImplicitRegisterAddress()
{
    auto resultContent = _alu->InstructionData.value().MemoryResult1;
    auto resultAddress = static_cast<uint16_t>(0xFF << 8 | _registers->Read(_alu->InstructionData.value().DestinationRegister));
    _memoryController->Write(static_cast<uint8_t>(resultContent), resultAddress);
}

inline void ControlUnit::WriteBackAtImplicitImmediateAddress()
{
    volatile auto resultContent = _registers->Read(_alu->InstructionData.value().SourceRegister);
    volatile auto resultAddress = static_cast<uint16_t>(0xFF << 8 | _alu->InstructionData.value().MemoryOperand1);
    _memoryController->Write(static_cast<uint8_t>(resultContent), resultAddress);
}

inline void ControlUnit::IncrementRegisterPair(Register reg)
{
    auto currentValue = _registers->ReadPair(reg);
    _registers->WritePair(reg, currentValue + 1);
}

inline void ControlUnit::DecrementRegisterPair(Register reg)
{
    auto currentValue = _registers->ReadPair(reg);
    _registers->WritePair(reg, currentValue - 1);
}

}