#include "InstructionBit.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::instructions
{

void InstructionBit::Decode(uint8_t opcode, [[maybe_unused]] optional<uint8_t> preOpcode, DecodedInstruction&decodedInstruction)
 {
    if ((opcode & 0x07) == 0x06) // Bit b, (HL)
        DecodeBitRegisterIndirectMode(opcode, decodedInstruction);
    else // Bit b, r
        DecodeBitRegisterMode(opcode, decodedInstruction);
}

void InstructionBit::Execute(RegisterBankInterface* registerBank, DecodedInstruction& decodedInstruction)
{
    auto operandValue = AcquireOperand(registerBank, decodedInstruction);
    auto result = static_cast<uint8_t>((operandValue >> decodedInstruction.InstructionExtraOperand) & 0x01);
    SetFlags(result, registerBank);
}

inline uint8_t InstructionBit::AcquireOperand(RegisterBankInterface* registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.AddressingMode == AddressingMode::Register)
        return registerBank->Read(decodedInstruction.SourceRegister);
    else
        return decodedInstruction.MemoryOperand1;
}

inline void InstructionBit::DecodeBitRegisterMode(uint8_t opcode, interfaces::DecodedInstruction& decodedInstruction)
{
    auto operand = RegisterBankInterface::FromInstructionSource((opcode & 0x7));
    auto targetBit = static_cast<uint8_t>((opcode >> 3) & 0x7);
    
    decodedInstruction =
    {
        .Opcode = OpcodeType::bit,
        .AddressingMode = AddressingMode::Register,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = operand,
        .DestinationRegister = operand,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = targetBit
    };    
}

inline void InstructionBit::DecodeBitRegisterIndirectMode(uint8_t opcode, interfaces::DecodedInstruction& decodedInstruction)
{
    auto targetBit = static_cast<uint8_t>((opcode >> 3) & 0x7);

    decodedInstruction =
    {
        .Opcode = OpcodeType::bit,
        .AddressingMode = AddressingMode::RegisterIndirectSource,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::HL,
        .DestinationRegister = Register::HL,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = targetBit
    };
}

inline void InstructionBit::SetFlags(uint8_t result, RegisterBankInterface* registerBank)
{
    registerBank->WriteFlag(Flag::Z, (result == 0? 0x01 : 0x00));
    registerBank->ClearFlag(Flag::N);
    registerBank->SetFlag(Flag::H);
}

}