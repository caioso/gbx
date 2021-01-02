#include "ArithmeticLogicUnit.h"

using namespace std;

namespace gbx
{

ArithmeticLogicUnit::ArithmeticLogicUnit()
    : _registers(make_shared<RegisterBank>())
    , _preOpcode(nullopt)
    , _shouldAcquireOperand(false)
    , _shouldWriteBack(false)
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
    if (_shouldAcquireOperand)
        AcquireOperand1();
    
    // 2.2 Acquire Operand 2 or Operand from address + displacement or MSByte of 
    if (_shouldAcquireOperand)
        AcquireOperand2();

    // 2.3 Acquire Operand 3 (extended source) 
    if (_shouldAcquireOperand)
        AcquireOperand3();

    // 3 Execute
    Execute();

    // 4 WriteBack
    if (_shouldWriteBack)
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
    DecideToAcquireOrExecute();
}

inline void ArithmeticLogicUnit::AcquireOperand1()
{
    _shouldAcquireOperand = false;
    
    if (auto currentAddressingMode = _currentInstruction->InstructionData.value().AddressingMode;
        InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::Immediate, 
                                                                      AddressingMode::RegisterIndexedSource, 
                                                                      AddressingMode::RegisterIndexedDestination,
                                                                      AddressingMode::ExtendedSource, 
                                                                      AddressingMode::ExtendedDestination))
    {
        _currentInstruction->InstructionData.value().MemoryOperand1 = ReadAtRegister(Register::PC);
        IncrementPC();

        if (InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::RegisterIndexedSource, 
                                                                          AddressingMode::ExtendedSource,
                                                                          AddressingMode::ExtendedDestination))
            _shouldAcquireOperand = true;
    }
    else if (InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::RegisterIndirectSource))
        _currentInstruction->InstructionData.value().MemoryOperand1 = ReadAtRegister(_currentInstruction->InstructionData.value().SourceRegister);
    

}

inline void ArithmeticLogicUnit::AcquireOperand2()
{
    if (auto currentAddressingMode = _currentInstruction->InstructionData.value().AddressingMode;
        InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::RegisterIndexedSource))
    {
        _shouldAcquireOperand = false;
        auto operandLocation = static_cast<uint16_t>(static_cast<int8_t>(_currentInstruction->InstructionData.value().MemoryOperand1) + 
                                                                         _registers->ReadPair(_currentInstruction->InstructionData.value().SourceRegister));
        _currentInstruction->InstructionData.value().MemoryOperand2 = get<uint8_t>(_memoryController->Read(operandLocation, MemoryAccessType::Byte));
    }
    else // Extended
    {
        _currentInstruction->InstructionData.value().MemoryOperand2 = ReadAtRegister(Register::PC);
        IncrementPC();

        if (InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::ExtendedDestination))
            _shouldAcquireOperand = false;
    }
}

inline void ArithmeticLogicUnit::AcquireOperand3()
{
    _shouldAcquireOperand = false;
    auto operandLocation = static_cast<uint16_t>(_currentInstruction->InstructionData.value().MemoryOperand1 | 
                                                 _currentInstruction->InstructionData.value().MemoryOperand2 << 8);
    _currentInstruction->InstructionData.value().MemoryOperand3 = get<uint8_t>(_memoryController->Read(operandLocation, MemoryAccessType::Byte));
}

inline void ArithmeticLogicUnit::Execute()
{
    ExecuteInstruction();
    DecideToWriteBackOrFetchPC();
}

inline void ArithmeticLogicUnit::WriteBack()
{
    _shouldWriteBack = false;
    WriteBackResults();
}

inline void ArithmeticLogicUnit::WriteBackResults()
{
    auto resultContent = _currentInstruction->InstructionData.value().MemoryResult1;
    
    if (auto currentAddressingMode = _currentInstruction->InstructionData.value().AddressingMode;
        InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::RegisterIndexedDestination))
    {
        auto resultAddress = static_cast<uint16_t>(static_cast<int8_t>(_currentInstruction->InstructionData.value().MemoryOperand1) + 
            _registers->ReadPair(_currentInstruction->InstructionData.value().DestinationRegister));
        _memoryController->Write(static_cast<uint8_t>(resultContent), resultAddress);
    }
    else if (InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::RegisterIndirectDestination))
    {
        auto resultAddress = _registers->ReadPair(_currentInstruction->InstructionData.value().DestinationRegister);
        _memoryController->Write(static_cast<uint8_t>(resultContent), resultAddress);
    }
    else // Extended
    {
        auto resultAddress = static_cast<uint16_t>(_currentInstruction->InstructionData.value().MemoryOperand1 | 
                                                   _currentInstruction->InstructionData.value().MemoryOperand2 << 8);
        _memoryController->Write(static_cast<uint8_t>(resultContent), resultAddress);
    }
}

inline void ArithmeticLogicUnit::ExecuteInstruction()
{
    _currentInstruction->Execute(_registers);
}

inline void ArithmeticLogicUnit::DecideToAcquireOrExecute()
{
    if (auto currentAddressingMode = _currentInstruction->InstructionData.value().AddressingMode;
        InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::Immediate, AddressingMode::RegisterIndirectSource, 
                                                                      AddressingMode::RegisterIndexedSource, AddressingMode::RegisterIndexedDestination,
                                                                      AddressingMode::ExtendedSource,
                                                                      AddressingMode::ExtendedDestination))
        _shouldAcquireOperand = true;
}

inline void ArithmeticLogicUnit::DecideToWriteBackOrFetchPC()
{
    if (auto currentAddressingMode = _currentInstruction->InstructionData.value().AddressingMode;
        InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::RegisterIndirectDestination, 
                                                                      AddressingMode::RegisterIndexedDestination,
                                                                      AddressingMode::ExtendedDestination))
        _shouldWriteBack = true;
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
    if (prefix == 0x01 || prefix == 0x00)
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

}