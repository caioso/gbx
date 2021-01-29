#include "ArithmeticLogicUnit.h"

using namespace std;
using namespace gbx::interfaces;

namespace gbx
{

ArithmeticLogicUnit::ArithmeticLogicUnit()
    : _currentAddressingMode(nullptr)
{}

void ArithmeticLogicUnit::Initialize(shared_ptr<RegisterBankInterface> registers)
{
    _registers = registers;
}

void ArithmeticLogicUnit::InitializeRegisters()
{
    _registers->Write(Register::IR, 0x00);
    _registers->WritePair(Register::PC, 0x0000);
    _registers->Write(Register::F, 0x00);
}

void ArithmeticLogicUnit::Decode()
{
    try
    {
        auto opcode = _registers->Read(Register::IR);
        auto complement = _registers->Read(Register::PIR);
        auto preOpcode = IsSuffixedInstruction(complement)? make_optional<uint8_t>(complement) : nullopt;
        _currentInstruction = _decoder.DecodeOpcode(opcode, preOpcode);
        _currentInstruction->Decode(opcode, preOpcode, _instructionData);
    }
    catch (const InstructionException&)
    {
        _currentInstruction.reset();
    }
}

uint8_t ArithmeticLogicUnit::AcquireInstruction(shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    auto instruction = ReadAtRegister(Register::PC, memoryController);
    IncrementPC();

    if (IsSuffixedInstruction(instruction))
    {
        auto secondInstruction = ReadAtRegister(Register::PC, memoryController);
        IncrementPC();

        _registers->Write(Register::PIR, instruction);
        _registers->Write(Register::IR, secondInstruction);
    }
    else 
    {
        _registers->Write(Register::PIR, 0x00);
        _registers->Write(Register::IR, instruction);
    }

    
    return instruction;
}

inline bool ArithmeticLogicUnit::IsSuffixedInstruction(uint8_t instruction)
{
    return instruction == InstructionConstants::PreOpcode_DD ||
           instruction == InstructionConstants::PreOpcode_CB ||
           instruction == InstructionConstants::PreOpcode_FD;
}

void ArithmeticLogicUnit::Execute()
{
    if (_currentInstruction == nullptr)
        throw InstructionException("tried to execute an andecoded instruction");

    _currentInstruction->Execute(_registers, _instructionData);     
}

AddressingModeFormat* ArithmeticLogicUnit::AcquireAddressingModeTraits()
{
    switch (_instructionData.AddressingMode)
    {
        case AddressingMode::Register: _currentAddressingMode = &AddressingModeTemplate::RegisterAddressingMode; break;
        case AddressingMode::Immediate: _currentAddressingMode = &AddressingModeTemplate::ImmediateAddressingMode; break;
        case AddressingMode::SingleImmediatePair: _currentAddressingMode = &AddressingModeTemplate::SingleImmediatePairAddressingMode; break;
        case AddressingMode::RegisterIndexedSource: _currentAddressingMode = &AddressingModeTemplate::RegisterIndexedSourceAddressingMode; break;
        case AddressingMode::RegisterIndexedDestination: _currentAddressingMode = &AddressingModeTemplate::RegisterIndexedDestinationAddressingMode; break;
        case AddressingMode::RegisterIndirectSource: _currentAddressingMode = &AddressingModeTemplate::RegisterIndirectSourceAddressingMode; break;
        case AddressingMode::RegisterIndirectDestination: _currentAddressingMode = &AddressingModeTemplate::RegisterIndirectDestinationAddressingMode; break;
        case AddressingMode::RegisterIndirectSourceAndDestination: _currentAddressingMode = &AddressingModeTemplate::RegisterIndirectSourceAndDestinationAddressingMode; break;
        case AddressingMode::ExtendedSource: _currentAddressingMode = &AddressingModeTemplate::RegisterExtendedSourceAddressingMode; break;
        case AddressingMode::ExtendedDestination: _currentAddressingMode = &AddressingModeTemplate::RegisterExtendedDestinationAddressingMode; break;
        case AddressingMode::ImmediateRegisterIndirect: _currentAddressingMode = &AddressingModeTemplate::ImmediateRegisterIndirectAddressingMode; break;
        case AddressingMode::RegisterIndirectSourceIncrement: _currentAddressingMode = &AddressingModeTemplate::ImmediateRegisterIndirectSourceIncrementAddressingMode; break;
        case AddressingMode::RegisterIndirectSourceDecrement: _currentAddressingMode = &AddressingModeTemplate::ImmediateRegisterIndirectSourceDecrementAddressingMode; break;
        case AddressingMode::RegisterIndirectDestinationIncrement: _currentAddressingMode = &AddressingModeTemplate::ImmediateRegisterIndirectDestinationIncrementAddressingMode; break;
        case AddressingMode::RegisterIndirectDestinationDecrement: _currentAddressingMode = &AddressingModeTemplate::ImmediateRegisterIndirectDestinationDecrementAddressingMode; break;
        case AddressingMode::RegisterImplicitSource: _currentAddressingMode = &AddressingModeTemplate::ImplicitRegisterSourceAddressingMode; break;
        case AddressingMode::RegisterImplicitDestination: _currentAddressingMode = &AddressingModeTemplate::ImplicitRegisterDestinationAddressingMode; break;
        case AddressingMode::ImmediateImplicitSource: _currentAddressingMode = &AddressingModeTemplate::ImplicitImmediateSourceAddressingMode; break;
        case AddressingMode::ImmediateImplicitDestination: _currentAddressingMode = &AddressingModeTemplate::ImplicitImmediateDestinationAddressingMode; break;
        case AddressingMode::ImmediatePair: _currentAddressingMode = &AddressingModeTemplate::ImmediatePairAddressingMode; break;
        case AddressingMode::RegisterPair: _currentAddressingMode = &AddressingModeTemplate::RegisterPairAddressingMode; break;
        case AddressingMode::RegisterIndirectDestinationPair: _currentAddressingMode = &AddressingModeTemplate::RegisterIndirectDestinationPair; break;
        case AddressingMode::RegisterIndirectSourcePair: _currentAddressingMode = &AddressingModeTemplate::RegisterIndirectSourcePair; break;
        case AddressingMode::ExtendedDestinationPair: _currentAddressingMode = &AddressingModeTemplate::ExtendedDestinationPair; break;
        default:
            throw ArithmeticLogicUnitException("invalid addressing mode");
    }

    return _currentAddressingMode;
}

void ArithmeticLogicUnit::AcquireOperand1AtPC(shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    _instructionData.MemoryOperand1 = ReadAtRegister(Register::PC, memoryController);
    IncrementPC();
}

void ArithmeticLogicUnit::AcquireOperand1AtRegister(shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    _instructionData.MemoryOperand1 = ReadAtRegister(_instructionData.SourceRegister, memoryController);

    if (_currentAddressingMode->incrementSource)
        IncrementRegisterPair(_instructionData.SourceRegister);
    else if (_currentAddressingMode->decrementSource)
        DecrementRegisterPair(_instructionData.SourceRegister);
}

void ArithmeticLogicUnit::AcquireOperand1Implicitly(shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    auto operandLocation = static_cast<uint16_t>(0xFF << 8 | _registers->Read(_instructionData.SourceRegister));
    _instructionData.MemoryOperand1 = get<uint8_t>(memoryController->Read(operandLocation, MemoryAccessType::Byte));
}

void ArithmeticLogicUnit::AcquireOperand2AtPC(shared_ptr<interfaces::MemoryControllerInterface> memoryContorller)
{
    _instructionData.MemoryOperand2 = ReadAtRegister(Register::PC, memoryContorller);
    IncrementPC();
}

void ArithmeticLogicUnit::AcquireOperand2AtComposedAddress(shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    auto operandLocation = static_cast<uint16_t>(static_cast<int8_t>(_instructionData.MemoryOperand1) + _registers->ReadPair(_instructionData.SourceRegister));
    _instructionData.MemoryOperand2 = get<uint8_t>(memoryController->Read(operandLocation, MemoryAccessType::Byte));
}

void ArithmeticLogicUnit::AcquireOperand2Implicitly(__attribute__((unused)) shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    auto operandLocation = static_cast<uint16_t>(0xFF << 8 | _instructionData.MemoryOperand1);
    _instructionData.MemoryOperand2 = get<uint8_t>(memoryController->Read(operandLocation, MemoryAccessType::Byte));
}

void ArithmeticLogicUnit::AcquireOperand2Directly(shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    _instructionData.MemoryOperand2 = ReadAtRegister(_instructionData.SourceRegister, memoryController);

    if (_currentAddressingMode->incrementSourceOperand2)
        IncrementRegisterPair(_instructionData.SourceRegister);
}

void ArithmeticLogicUnit::AcquireOperand3(shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    auto operandLocation = static_cast<uint16_t>(_instructionData.MemoryOperand1 | _instructionData.MemoryOperand2 << 8);
    _instructionData.MemoryOperand3 = get<uint8_t>(memoryController->Read(operandLocation, MemoryAccessType::Byte));
}

void ArithmeticLogicUnit::WriteBackAtOperandAddress(shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    auto resultContent = _instructionData.MemoryResult1;
    auto resultAddress = static_cast<uint16_t>(static_cast<int8_t>(_instructionData.MemoryOperand1) + _registers->ReadPair(_instructionData.DestinationRegister));
    memoryController->Write(static_cast<uint8_t>(resultContent), resultAddress);
}

void ArithmeticLogicUnit::WriteBackAtRegisterAddress(shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    auto resultContent = _instructionData.MemoryResult1;
    auto resultAddress = _registers->ReadPair(_instructionData.DestinationRegister);
    memoryController->Write(static_cast<uint8_t>(resultContent), resultAddress);

     if (_currentAddressingMode->incrementDestination)
        IncrementRegisterPair(_instructionData.DestinationRegister);
    else if (_currentAddressingMode->decrementDestination)
        DecrementRegisterPair(_instructionData.DestinationRegister);
}

void ArithmeticLogicUnit::WriteBackAtComposedAddress(__attribute__((unused)) shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    auto resultContent = _instructionData.MemoryResult1;
    auto resultAddress = static_cast<uint16_t>(_instructionData.MemoryOperand1 | _instructionData.MemoryOperand2 << 8);
    memoryController->Write(static_cast<uint8_t>(resultContent), resultAddress);
}

void ArithmeticLogicUnit::WriteBackAtImplicitRegisterAddress(shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    auto resultContent = _instructionData.MemoryResult1;
    auto resultAddress = static_cast<uint16_t>(0xFF << 8 | _registers->Read(_instructionData.DestinationRegister));
    memoryController->Write(static_cast<uint8_t>(resultContent), resultAddress);
}

void ArithmeticLogicUnit::WriteBackPairAtRegisterAddress(shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    auto operandMsb = _instructionData.MemoryResult1;
    auto operandLsb = _instructionData.MemoryResult2;
    auto stackPointer = _registers->ReadPair(Register::SP);
    memoryController->Write(static_cast<uint8_t>(operandLsb), stackPointer);
    memoryController->Write(static_cast<uint8_t>(operandMsb), stackPointer - 1);
    _registers->WritePair(Register::SP, stackPointer - 2);
}

void ArithmeticLogicUnit::WriteBackPairAtImmediareAddress(shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    auto operandLsb = _instructionData.MemoryResult1;
    auto operandMsb = _instructionData.MemoryResult2;
    auto address = static_cast<uint16_t>(_instructionData.MemoryOperand1 | (_instructionData.MemoryOperand2 << 0x08));
    memoryController->Write(static_cast<uint8_t>(operandLsb), address);
    memoryController->Write(static_cast<uint8_t>(operandMsb), address + 1);
}


void ArithmeticLogicUnit::WriteBackAtImplicitImmediateAddress(shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    auto resultContent = _registers->Read(_instructionData.SourceRegister);
    auto resultAddress = static_cast<uint16_t>(0xFF << 8 | _instructionData.MemoryOperand1);
    memoryController->Write(static_cast<uint8_t>(resultContent), resultAddress);
}

inline uint8_t ArithmeticLogicUnit::ReadAtRegister(Register reg, shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    auto registerContent = _registers->ReadPair(reg);
    return get<uint8_t>(memoryController->Read(registerContent, MemoryAccessType::Byte));
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

inline void ArithmeticLogicUnit::IncrementPC()
{
    auto programCounter = _registers->ReadPair(Register::PC);
    _registers->WritePair(Register::PC, programCounter + 1);
}

}