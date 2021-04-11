#include "InstructionRes.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::instructions
{

void InstructionRes::Decode(uint8_t opcode, [[maybe_unused]] optional<uint8_t> preOpcode, DecodedInstruction&decodedInstruction)
 {
    if ((opcode & 0x07) == 0x06) // Res b, (HL)
        DecodeResRegisterIndirectMode(opcode, decodedInstruction);
    else // Res b, r
        DecodeResRegisterMode(opcode, decodedInstruction);
}

void InstructionRes::Execute(RegisterBankInterface* registerBank, DecodedInstruction& decodedInstruction)
{
    auto operandValue = AcquireOperand(registerBank, decodedInstruction);
    auto result = static_cast<uint8_t>(operandValue & (~(0x01 << decodedInstruction.InstructionExtraOperand)));

    WriteResult(result, registerBank, decodedInstruction);
}

inline void InstructionRes::WriteResult(uint8_t result, RegisterBankInterface* registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.AddressingMode == AddressingMode::Register)
        registerBank->Write(decodedInstruction.DestinationRegister, result);
    else
        decodedInstruction.MemoryResult1 = result;
}

inline uint8_t InstructionRes::AcquireOperand(RegisterBankInterface* registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.AddressingMode == AddressingMode::Register)
        return registerBank->Read(decodedInstruction.SourceRegister);
    else
        return decodedInstruction.MemoryOperand1;
}

inline void InstructionRes::DecodeResRegisterMode(uint8_t opcode, interfaces::DecodedInstruction& decodedInstruction)
{
    auto operand = RegisterBankInterface::FromInstructionSource((opcode & 0x7));
    auto targetBit = static_cast<uint8_t>((opcode >> 3) & 0x7);
    
    decodedInstruction =
    {
        .Opcode = OpcodeType::res,
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

inline void InstructionRes::DecodeResRegisterIndirectMode(uint8_t opcode, interfaces::DecodedInstruction& decodedInstruction)
{
    auto targetBit = static_cast<uint8_t>((opcode >> 3) & 0x7);

    decodedInstruction =
    {
        .Opcode = OpcodeType::res,
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