#include "InstructionSub.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::instructions
{

void InstructionSub::Decode(uint8_t opcode, [[maybe_unused]] optional<uint8_t> preOpcode, DecodedInstruction& decodedInstruction) 
{
    if (opcode == 0xD6)
        DecodeSubImmediateMode(decodedInstruction);
    else if (opcode == 0x96)
        DecodeSubRegisterIndirectMode(decodedInstruction);
    else if ((opcode >> 0x03) == 0x12)
        DecodeSubRegisterMode(opcode, decodedInstruction);
}

void InstructionSub::Execute(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    auto operand1  = registerBank->Read(decodedInstruction.DestinationRegister); // Always A
    auto operand2 = Acquire8bitSourceOperandValue(registerBank, decodedInstruction);
    
    registerBank->Write(Register::F, 0x00);
    auto result = CalculateBinarySubtractionAndSetFlags(operand1, operand2, nullopt, registerBank);
    registerBank->Write(decodedInstruction.DestinationRegister, static_cast<uint8_t>(result));
}

inline void InstructionSub::DecodeSubRegisterMode(uint8_t opcode, DecodedInstruction& decodedInstruction)
{
    auto source = RegisterBankInterface::FromInstructionSource(opcode & 0x07);
    decodedInstruction =
    {
        .Opcode = OpcodeType::sub,
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

inline void InstructionSub::DecodeSubImmediateMode(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::sub,
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

inline void InstructionSub::DecodeSubRegisterIndirectMode(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::sub,
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