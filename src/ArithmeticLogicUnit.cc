#include "ArithmeticLogicUnit.h"

using namespace std;

namespace gbx
{

ArithmeticLogicUnit::ArithmeticLogicUnit()
    : _registers(make_shared<RegisterBank>())
    , _preOpcode(nullopt)
    , _currentAddressingMode(make_shared<AddressingModeFormat>(AddressingModeTemplate::NoAddressingMode))
{
    InitializeRegisters();
}

void ArithmeticLogicUnit::Initialize(shared_ptr<MemoryControllerInterface> memoryController)
{
    _memoryController = memoryController;
}

void ArithmeticLogicUnit::RunCycle()
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

inline void ArithmeticLogicUnit::Fetch()
{
    auto instruction = ReadAtRegister(Register::PC);
    IncrementPC();

    if (instruction == 0xDD || instruction == 0xFD)
        CompletePreOpcodeFetch(instruction);
    else
        CompleteFetchPC(instruction);
}

inline void ArithmeticLogicUnit::FetchAgain()
{
    auto instruction = ReadAtRegister(Register::PC);
    IncrementPC();
    CompleteFetchPC(instruction);
}

inline void ArithmeticLogicUnit::Decode()
{
    DecodeInstruction();
    AcquireAddressingMode();
}

inline void ArithmeticLogicUnit::AcquireOperand1()
{
    if (_currentAddressingMode->acquireOperand1FromPc)
        ReadOperand1AtPC();
    else if (_currentAddressingMode->acquireOperand1Directly)
        ReadOperand1AtRegister();
}

inline void ArithmeticLogicUnit::AcquireOperand2()
{
    if (_currentAddressingMode->acquireOperand2FromPc)
        ReadOperand2AtPC();
    else if (_currentAddressingMode->acquireOperand2AtComposedAddress)
        ReadOperand2AtComposedAddress();
}

inline void ArithmeticLogicUnit::AcquireOperand3()
{
    auto operandLocation = static_cast<uint16_t>(_currentInstruction->InstructionData.value().MemoryOperand1 | _currentInstruction->InstructionData.value().MemoryOperand2 << 8);
    _currentInstruction->InstructionData.value().MemoryOperand3 = get<uint8_t>(_memoryController->Read(operandLocation, MemoryAccessType::Byte));
}

inline void ArithmeticLogicUnit::Execute()
{
    ExecuteInstruction();
}

inline void ArithmeticLogicUnit::WriteBack()
{
    WriteBackResults();
}

inline void ArithmeticLogicUnit::InitializeRegisters()
{
    _registers->Write(Register::IR, 0x00);
    _registers->WritePair(Register::PC, 0x0000);
    _registers->Write(Register::F, 0x00);
}

inline uint8_t ArithmeticLogicUnit::ReadAtRegister(Register reg)
{
    auto registerContent = _registers->ReadPair(reg);
    return get<uint8_t>(_memoryController->Read(registerContent, MemoryAccessType::Byte));
}

inline void ArithmeticLogicUnit::IncrementPC()
{
    auto programCounter = _registers->ReadPair(Register::PC);
    _registers->WritePair(Register::PC, programCounter + 1);
}

inline void ArithmeticLogicUnit::CompleteFetchPC(uint8_t instruction)
{
    _registers->Write(Register::IR, instruction);
}

inline void ArithmeticLogicUnit::CompletePreOpcodeFetch(uint8_t preOpcode)
{
    _preOpcode = preOpcode;
}

inline void ArithmeticLogicUnit::DecodeInstruction()
{
    auto opcode = _registers->Read(Register::IR);
    InstantiateInstruction(opcode);
    _preOpcode = nullopt;
}

inline void ArithmeticLogicUnit::InstantiateInstruction(uint8_t opcode)
{
    auto prefix = (opcode >> 6);

    // Improve this
    if (prefix == 0x01 || prefix == 0x00 || prefix == 0x03)
    {
        _currentInstruction = make_unique<LD>();
        _currentInstruction->Decode(opcode, _preOpcode);
    }
    else
    {
        stringstream ss;
        throw ArithmeticLogicUnitException(ss.str());
    }
}

inline void ArithmeticLogicUnit::AcquireAddressingMode()
{
    switch (_currentInstruction->InstructionData.value().AddressingMode)
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
        default:
            throw ArithmeticLogicUnitException("invalid addressing mode");
    }
}

inline void ArithmeticLogicUnit::ReadOperand1AtPC()
{
    _currentInstruction->InstructionData.value().MemoryOperand1 = ReadAtRegister(Register::PC);
    IncrementPC();
}

inline void ArithmeticLogicUnit::ReadOperand1AtRegister()
{
    _currentInstruction->InstructionData.value().MemoryOperand1 = ReadAtRegister(_currentInstruction->InstructionData.value().SourceRegister);

    if (_currentAddressingMode->incrementSource)
        IncrementRegisterPair(_currentInstruction->InstructionData.value().SourceRegister);
    else if (_currentAddressingMode->decrementSource)
        DecrementRegisterPair(_currentInstruction->InstructionData.value().SourceRegister);
}

inline void ArithmeticLogicUnit::ReadOperand2AtPC()
{
    _currentInstruction->InstructionData.value().MemoryOperand2 = ReadAtRegister(Register::PC);
    IncrementPC();
}

inline void ArithmeticLogicUnit::ReadOperand2AtComposedAddress()
{
    auto operandLocation = static_cast<uint16_t>(static_cast<int8_t>(_currentInstruction->InstructionData.value().MemoryOperand1) + _registers->ReadPair(_currentInstruction->InstructionData.value().SourceRegister));
    _currentInstruction->InstructionData.value().MemoryOperand2 = get<uint8_t>(_memoryController->Read(operandLocation, MemoryAccessType::Byte));
}

inline void ArithmeticLogicUnit::ExecuteInstruction()
{
    _currentInstruction->Execute(_registers);
}

inline void ArithmeticLogicUnit::WriteBackResults()
{
    if (_currentAddressingMode->writeBackAtOperandAddress)
        WriteBackAtOperandAddress();
    else if (_currentAddressingMode->writeBackAtRegisterAddress)
        WriteBackAtRegisterAddress();
    else if (_currentAddressingMode->writeBackAtComposedOperandAddress)
        WriteBackAtComposedAddress();
}

inline void ArithmeticLogicUnit::WriteBackAtOperandAddress()
{
    auto resultContent = _currentInstruction->InstructionData.value().MemoryResult1;
    auto resultAddress = static_cast<uint16_t>(static_cast<int8_t>(_currentInstruction->InstructionData.value().MemoryOperand1) + _registers->ReadPair(_currentInstruction->InstructionData.value().DestinationRegister));
    _memoryController->Write(static_cast<uint8_t>(resultContent), resultAddress);
}

inline void ArithmeticLogicUnit::WriteBackAtRegisterAddress()
{
    auto resultContent = _currentInstruction->InstructionData.value().MemoryResult1;
    auto resultAddress = _registers->ReadPair(_currentInstruction->InstructionData.value().DestinationRegister);
    _memoryController->Write(static_cast<uint8_t>(resultContent), resultAddress);

     if (_currentAddressingMode->incrementDestination)
        IncrementRegisterPair(_currentInstruction->InstructionData.value().DestinationRegister);
    else if (_currentAddressingMode->decrementDestination)
        DecrementRegisterPair(_currentInstruction->InstructionData.value().DestinationRegister);
}

inline void ArithmeticLogicUnit::WriteBackAtComposedAddress()
{
    auto resultContent = _currentInstruction->InstructionData.value().MemoryResult1;
    auto resultAddress = static_cast<uint16_t>(_currentInstruction->InstructionData.value().MemoryOperand1 | _currentInstruction->InstructionData.value().MemoryOperand2 << 8);
    _memoryController->Write(static_cast<uint8_t>(resultContent), resultAddress);
}

inline void ArithmeticLogicUnit::IncrementRegisterPair(Register reg)
{
    auto currentValue = _registers->ReadPair(reg);
    _registers->WritePair(reg, currentValue + 1);
}

inline void ArithmeticLogicUnit::DecrementRegisterPair(Register reg)
{
    auto currentValue = _registers->ReadPair(reg);
    _registers->WritePair(reg, currentValue - 1);
}

}