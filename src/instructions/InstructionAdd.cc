#include "InstructionAdd.h"

using namespace std;
using namespace gbx::interfaces;

namespace gbx
{

void InstructionAdd::Decode(uint8_t opcode, __attribute__((unused)) optional<uint8_t> preOpcode, DecodedInstruction& decodeInstruction)
{
    if (opcode == 0x86)  // ADD A, (HL)
        DecodeAddPointerMode(decodeInstruction);
    else if (opcode == 0xE8)  // ADD SP, ss
        DecodeAddImmediatePairMode(decodeInstruction);
    else if (opcode == 0xC6)  // ADD A, #XX
        DecodeAddImmediateMode(decodeInstruction);
    else if ((opcode >> 0x03) == 0x10)  // ADD A, r
        DecodeAddRegisterMode(opcode, decodeInstruction);
    else if ((opcode >> 0x06) == 0x00 && (opcode & 0x0F) == 0x09)  // ADD HL, ss
        DecodeAddRegisterPairMode(opcode, decodeInstruction);
}

void InstructionAdd::Execute(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.AddressingMode == AddressingMode::RegisterPair || 
        decodedInstruction.AddressingMode == AddressingMode::SingleImmediatePair)
        Add16Bit(registerBank, decodedInstruction);
    else 
        Add8Bit(registerBank, decodedInstruction);
}

void InstructionAdd::Add8Bit(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    auto operand1 = Acquire8BitSourceOperandValue(registerBank, decodedInstruction);
    auto operand2  = registerBank->Read(decodedInstruction.DestinationRegister); // Always A
    
    registerBank->Write(Register::F, 0x00);
    auto result = Calculate8BitBinaryAdditionAndSetFlags(operand1, operand2, nullopt, registerBank);
    registerBank->Write(decodedInstruction.DestinationRegister, static_cast<uint8_t>(result));
}

void InstructionAdd::Add16Bit(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    auto operand1 = Acquire16BitSourceOperandValue(registerBank, decodedInstruction);
    auto operand2  = registerBank->ReadPair(decodedInstruction.DestinationRegister); // Always HL
    auto flagMode = DecideFlagMode(decodedInstruction);
    auto zFlag = registerBank->ReadFlag(Flag::Z);    

    registerBank->Write(Register::F, 0x00);
    auto result = Calculate16BitBinaryAdditionAndSetFlags(operand1, operand2, nullopt, registerBank, flagMode);
    registerBank->WritePair(decodedInstruction.DestinationRegister, static_cast<uint16_t>(result));

    if (decodedInstruction.AddressingMode == AddressingMode::SingleImmediatePair) // Valid for ADD SP, ss
        registerBank->ClearFlag(Flag::Z);
    if (decodedInstruction.AddressingMode == AddressingMode::RegisterPair) // Valid for ADD HL, qq
    {
        if (zFlag == 1)
            registerBank->SetFlag(Flag::Z);
        else
            registerBank->ClearFlag(Flag::Z);
    }
}

inline void InstructionAdd::DecodeAddRegisterMode(uint8_t opcode, DecodedInstruction& decodedInstruction)
{
    auto source = RegisterBankInterface::FromInstructionSource(opcode & 0x07);
    decodedInstruction =
    {
        .Opcode = OpcodeType::add,
        .AddressingMode = AddressingMode::Register,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = source,
        .DestinationRegister = Register::A,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0x00
    };
}

inline void InstructionAdd::DecodeAddRegisterPairMode(uint8_t opcode, DecodedInstruction& decodedInstruction)
{
    auto source = RegisterBankInterface::FromInstructionToPair((opcode >> 0x04) & 0x03);
    decodedInstruction =
    {
        .Opcode = OpcodeType::add,
        .AddressingMode = AddressingMode::RegisterPair,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = source,
        .DestinationRegister = Register::HL,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0x00
    };
}

inline void InstructionAdd::DecodeAddImmediateMode(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::add,
        .AddressingMode = AddressingMode::Immediate,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::NoRegister, 
        .DestinationRegister = Register::A,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0x00
    };
}

inline void InstructionAdd::DecodeAddImmediatePairMode(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::add,
        .AddressingMode = AddressingMode::SingleImmediatePair,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::NoRegister, 
        .DestinationRegister = Register::SP,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0x00
    };
}

inline void InstructionAdd::DecodeAddPointerMode(interfaces::DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::add,
        .AddressingMode = AddressingMode::RegisterIndirectSource,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::HL, 
        .DestinationRegister = Register::A,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0x00
    };
}


}