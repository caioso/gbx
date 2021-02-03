#include "InstructionSet.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx
{

void InstructionSet::Decode(uint8_t opcode, __attribute__((unused)) optional<uint8_t> preOpcode, DecodedInstruction&decodedInstruction)
 {
    if ((opcode & 0x07) == 0x06) // Set b, (HL)
        DecodeSetRegisterIndirectMode(opcode, decodedInstruction);
    else // Set b, r
        DecodeSetRegisterMode(opcode, decodedInstruction);
}

void InstructionSet::Execute(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction, __attribute__((unused)) bool& isWriteBackAborted)
{
    auto operandValue = AcquireOperand(registerBank, decodedInstruction);
    auto result = static_cast<uint8_t>(operandValue | (0x01 << decodedInstruction.InstructionExtraOperand));

    WriteResult(result, registerBank, decodedInstruction);
}

inline void InstructionSet::WriteResult(uint8_t result, shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.AddressingMode == AddressingMode::Register)
        registerBank->Write(decodedInstruction.DestinationRegister, result);
    else
        decodedInstruction.MemoryResult1 = result;
}

inline uint8_t InstructionSet::AcquireOperand(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.AddressingMode == AddressingMode::Register)
        return registerBank->Read(decodedInstruction.SourceRegister);
    else
        return decodedInstruction.MemoryOperand1;
}

inline void InstructionSet::DecodeSetRegisterMode(uint8_t opcode, interfaces::DecodedInstruction& decodedInstruction)
{
    auto operand = RegisterBankInterface::FromInstructionSource((opcode & 0x7));
    auto targetBit = static_cast<uint8_t>((opcode >> 3) & 0x7);
    
    decodedInstruction =
    {
        .Opcode = OpcodeType::set,
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

inline void InstructionSet::DecodeSetRegisterIndirectMode(uint8_t opcode, interfaces::DecodedInstruction& decodedInstruction)
{
    auto targetBit = static_cast<uint8_t>((opcode >> 3) & 0x7);

    decodedInstruction =
    {
        .Opcode = OpcodeType::set,
        .AddressingMode = AddressingMode::RegisterIndirectSourceAndDestination,
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

}