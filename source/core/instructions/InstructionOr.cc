#include "InstructionOr.h"

using namespace std;
using namespace gbxcore::interfaces;

namespace gbxcore::instructions
{
void InstructionOr::Decode(uint8_t opcode, [[maybe_unused]] std::optional<uint8_t> preOpcode, DecodedInstruction& decodedInstruction)
{
    if (opcode == 0xF6)
        DecodeOrImmediateMode(decodedInstruction);
    else if (opcode == 0xB6)
        DecodeOrRegisterIndirectMode(decodedInstruction);
    else if ((opcode >> 0x03) == 0x16)
        DecodeOrRegisterMode(opcode, decodedInstruction);
}

void InstructionOr::Execute(RegisterBankInterface* registerBank, DecodedInstruction& decodedInstruction) 
{
    auto operand1 = GetSourceOperandValue(registerBank, decodedInstruction);
    auto operand2 = registerBank->Read(decodedInstruction.DestinationRegister);
    registerBank->Write(decodedInstruction.DestinationRegister, operand1 | operand2);

    registerBank->ClearFlag(Flag::CY);
    registerBank->ClearFlag(Flag::H);
    registerBank->ClearFlag(Flag::N);

    if ((operand1 | operand2) == 0)
        registerBank->SetFlag(Flag::Z);
    else
        registerBank->ClearFlag(Flag::Z);
}

inline uint8_t InstructionOr::GetSourceOperandValue(RegisterBankInterface* registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.AddressingMode == AddressingMode::Register)
        return registerBank->Read(decodedInstruction.SourceRegister);
    else 
        return decodedInstruction.MemoryOperand1;
}

inline void InstructionOr::DecodeOrRegisterMode(uint8_t opcode, DecodedInstruction& decodedInstruction)
{
    auto source = RegisterBankInterface::FromInstructionSource(opcode & 0x07);
    decodedInstruction =
    {
        .Opcode = OpcodeType::bitwiseor,
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

inline void InstructionOr::DecodeOrImmediateMode(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::bitwiseor,
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

inline void InstructionOr::DecodeOrRegisterIndirectMode(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::bitwiseor,
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