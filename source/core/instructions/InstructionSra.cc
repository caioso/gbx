#include "InstructionSra.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::instructions
{

void InstructionSra::Decode(uint8_t opcode, [[maybe_unused]] optional<uint8_t> preOpcode, DecodedInstruction&decodedInstruction)
{
    if ((opcode & 0x07) == 0x06) // Sra (HL)
        DecodeSraRegisterIndirectMode(decodedInstruction);
    else // Sra r
        DecodeSraRegisterMode(opcode, decodedInstruction);
}

void InstructionSra::Execute(RegisterBankInterface* registerBank, DecodedInstruction& decodedInstruction)
{
    auto operandValue = AcquireOperand(registerBank, decodedInstruction);
    auto lsBit = static_cast<uint8_t>(operandValue & 0x01);    
    auto msBit = static_cast<uint8_t>((operandValue >> 0x07) & 0x01);    
    auto result = static_cast<uint8_t>(static_cast<uint8_t>(operandValue >> 1) | msBit << 7);

    SetFlags(result, registerBank, lsBit);
    WriteResult(result, registerBank, decodedInstruction);
}

inline void InstructionSra::WriteResult(uint8_t result, RegisterBankInterface* registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.AddressingMode == AddressingMode::Register)
        registerBank->Write(decodedInstruction.DestinationRegister, result);
    else
        decodedInstruction.MemoryResult1 = result;
}

inline uint8_t InstructionSra::AcquireOperand(RegisterBankInterface* registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.AddressingMode == AddressingMode::Register)
        return registerBank->Read(decodedInstruction.SourceRegister);
    else
        return decodedInstruction.MemoryOperand1;
}

inline void InstructionSra::DecodeSraRegisterMode(uint8_t opcode, interfaces::DecodedInstruction& decodedInstruction)
{
    auto operand = RegisterBankInterface::FromInstructionSource((opcode & 0x7));
    decodedInstruction =
    {
        .Opcode = OpcodeType::sra,
        .AddressingMode = AddressingMode::Register,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = operand,
        .DestinationRegister = operand,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0x00
    };    
}

inline void InstructionSra::DecodeSraRegisterIndirectMode(interfaces::DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::sra,
        .AddressingMode = AddressingMode::RegisterIndirectSourceAndDestination,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::HL,
        .DestinationRegister = Register::HL,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0x00
    };    
}

inline void InstructionSra::SetFlags(uint8_t result, RegisterBankInterface* registerBank, uint8_t flagValue)
{
    registerBank->WriteFlag(Flag::CY, flagValue);
    registerBank->WriteFlag(Flag::Z, (result == 0? 0x01 : 0x00));
    registerBank->ClearFlag(Flag::H);
    registerBank->ClearFlag(Flag::N);
}

}