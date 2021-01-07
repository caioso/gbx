#include "ArithmeticLogicUnit.h"

using namespace std;
using namespace gbx::interfaces;

namespace gbx
{

ArithmeticLogicUnit::ArithmeticLogicUnit()
    : _currentAddressingMode(nullptr)
{}

void ArithmeticLogicUnit::Decode(uint8_t opcode, std::optional<uint8_t> preOpcode)
{
    try
    {
        _currentInstruction = _decoder.DecodeOpcode(opcode, preOpcode);
        _currentInstruction->Decode(opcode, preOpcode, _instructionData);
    }
    catch (const InstructionException&)
    {
        _currentInstruction.reset();
    }
}

void ArithmeticLogicUnit::Execute(std::shared_ptr<RegisterBank> registerBank)
{
    if (_currentInstruction == nullptr)
        throw InstructionException("tried to execute an andecoded instruction");

    _currentInstruction->Execute(registerBank, _instructionData);     
}

AddressingModeFormat* ArithmeticLogicUnit::AcquireAddressingModeTraits()
{
    switch (_instructionData.AddressingMode)
    {
        case AddressingMode::Register: _currentAddressingMode = &AddressingModeTemplate::RegisterAddressingMode; break;
        case AddressingMode::Immediate: _currentAddressingMode = &AddressingModeTemplate::ImmediateAddressingMode; break;
        case AddressingMode::RegisterIndexedSource: _currentAddressingMode = &AddressingModeTemplate::RegisterIndexedSourceAddressingMode; break;
        case AddressingMode::RegisterIndexedDestination: _currentAddressingMode = &AddressingModeTemplate::RegisterIndexedDestinationAddressingMode; break;
        case AddressingMode::RegisterIndirectSource: _currentAddressingMode = &AddressingModeTemplate::RegisterIndirectSourceAddressingMode; break;
        case AddressingMode::RegisterIndirectDestination: _currentAddressingMode = &AddressingModeTemplate::RegisterIndirectDestinationAddressingMode; break;
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
        default:
            throw ArithmeticLogicUnitException("invalid addressing mode");
    }

    return _currentAddressingMode;
}

void ArithmeticLogicUnit::AcquireOperand1AtPC(std::shared_ptr<RegisterBank> registerBank, std::shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    _instructionData.MemoryOperand1 = ReadAtRegister(Register::PC, registerBank, memoryController);
    IncrementPC(registerBank);
}

void ArithmeticLogicUnit::AcquireOperand1AtRegister(std::shared_ptr<RegisterBank> registerBank, std::shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    _instructionData.MemoryOperand1 = ReadAtRegister(_instructionData.SourceRegister, registerBank, memoryController);

    if (_currentAddressingMode->incrementSource)
        IncrementRegisterPair(_instructionData.SourceRegister, registerBank);
    else if (_currentAddressingMode->decrementSource)
        DecrementRegisterPair(_instructionData.SourceRegister, registerBank);
}

void ArithmeticLogicUnit::AcquireOperand1Implicitly(std::shared_ptr<RegisterBank> registerBank, std::shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    auto operandLocation = static_cast<uint16_t>(0xFF << 8 | registerBank->Read(_instructionData.SourceRegister));
    _instructionData.MemoryOperand1 = get<uint8_t>(memoryController->Read(operandLocation, MemoryAccessType::Byte));
}

void ArithmeticLogicUnit::AcquireOperand2AtPC(std::shared_ptr<RegisterBank> registerBank, std::shared_ptr<interfaces::MemoryControllerInterface> memoryContorller)
{
    _instructionData.MemoryOperand2 = ReadAtRegister(Register::PC, registerBank, memoryContorller);
    IncrementPC(registerBank);
}

void ArithmeticLogicUnit::AcquireOperand2AtComposedAddress(std::shared_ptr<RegisterBank> registerBank, std::shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    auto operandLocation = static_cast<uint16_t>(static_cast<int8_t>(_instructionData.MemoryOperand1) + registerBank->ReadPair(_instructionData.SourceRegister));
    _instructionData.MemoryOperand2 = get<uint8_t>(memoryController->Read(operandLocation, MemoryAccessType::Byte));
}

void ArithmeticLogicUnit::AcquireOperand2Implicitly(__attribute__((unused)) std::shared_ptr<RegisterBank> registerBank, std::shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    auto operandLocation = static_cast<uint16_t>(0xFF << 8 | _instructionData.MemoryOperand1);
    _instructionData.MemoryOperand2 = get<uint8_t>(memoryController->Read(operandLocation, MemoryAccessType::Byte));
}

void ArithmeticLogicUnit::AcquireOperand3(std::shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    auto operandLocation = static_cast<uint16_t>(_instructionData.MemoryOperand1 | _instructionData.MemoryOperand2 << 8);
    _instructionData.MemoryOperand3 = get<uint8_t>(memoryController->Read(operandLocation, MemoryAccessType::Byte));
}

void ArithmeticLogicUnit::WriteBackAtOperandAddress(std::shared_ptr<RegisterBank> registerBank, std::shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    auto resultContent = _instructionData.MemoryResult1;
    auto resultAddress = static_cast<uint16_t>(static_cast<int8_t>(_instructionData.MemoryOperand1) + registerBank->ReadPair(_instructionData.DestinationRegister));
    memoryController->Write(static_cast<uint8_t>(resultContent), resultAddress);
}

void ArithmeticLogicUnit::WriteBackAtRegisterAddress(std::shared_ptr<RegisterBank> registerBank, std::shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    auto resultContent = _instructionData.MemoryResult1;
    auto resultAddress = registerBank->ReadPair(_instructionData.DestinationRegister);
    memoryController->Write(static_cast<uint8_t>(resultContent), resultAddress);

     if (_currentAddressingMode->incrementDestination)
        IncrementRegisterPair(_instructionData.DestinationRegister, registerBank);
    else if (_currentAddressingMode->decrementDestination)
        DecrementRegisterPair(_instructionData.DestinationRegister, registerBank);
}

void ArithmeticLogicUnit::WriteBackAtComposedAddress(__attribute__((unused)) std::shared_ptr<RegisterBank> registerBank, std::shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    auto resultContent = _instructionData.MemoryResult1;
    auto resultAddress = static_cast<uint16_t>(_instructionData.MemoryOperand1 | _instructionData.MemoryOperand2 << 8);
    memoryController->Write(static_cast<uint8_t>(resultContent), resultAddress);
}

void ArithmeticLogicUnit::WriteBackAtImplicitRegisterAddress(std::shared_ptr<RegisterBank> registerBank, std::shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    auto resultContent = _instructionData.MemoryResult1;
    auto resultAddress = static_cast<uint16_t>(0xFF << 8 | registerBank->Read(_instructionData.DestinationRegister));
    memoryController->Write(static_cast<uint8_t>(resultContent), resultAddress);
}

void ArithmeticLogicUnit::WriteBackAtImplicitImmediateAddress(std::shared_ptr<RegisterBank> registerBank, std::shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    volatile auto resultContent = registerBank->Read(_instructionData.SourceRegister);
    volatile auto resultAddress = static_cast<uint16_t>(0xFF << 8 | _instructionData.MemoryOperand1);
    memoryController->Write(static_cast<uint8_t>(resultContent), resultAddress);
}

inline uint8_t ArithmeticLogicUnit::ReadAtRegister(Register reg, std::shared_ptr<RegisterBank> registerBank, std::shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    auto registerContent = registerBank->ReadPair(reg);
    return get<uint8_t>(memoryController->Read(registerContent, MemoryAccessType::Byte));
}

inline void ArithmeticLogicUnit::IncrementRegisterPair(Register reg, std::shared_ptr<RegisterBank> registerBank)
{
    auto currentValue = registerBank->ReadPair(reg);
    registerBank->WritePair(reg, currentValue + 1);
}

inline void ArithmeticLogicUnit::DecrementRegisterPair(Register reg, std::shared_ptr<RegisterBank> registerBank)
{
    auto currentValue = registerBank->ReadPair(reg);
    registerBank->WritePair(reg, currentValue - 1);
}

inline void ArithmeticLogicUnit::IncrementPC(std::shared_ptr<RegisterBank> registerBank)
{
    auto programCounter = registerBank->ReadPair(Register::PC);
    registerBank->WritePair(Register::PC, programCounter + 1);
}

}