#include "InstructionRlc.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::instructions
{

void InstructionRlc::Decode(uint8_t opcode, [[maybe_unused]] optional<uint8_t> complement, DecodedInstruction&decodedInstruction)
{
    if (opcode == 0x06) // RLC (HL)
        DecodeRlcRegisterIndirectMode(decodedInstruction);
    else // RLC r
        DecodeRlcRegisterMode(opcode, decodedInstruction);       
}

void InstructionRlc::Execute(RegisterBankInterface* registerBank, DecodedInstruction& decodedInstruction)
{
    auto value = AcquireOperand(registerBank, decodedInstruction);
    auto valueMSbit = static_cast<uint8_t>((value >> 7) & 0x01);
    auto result = static_cast<uint8_t>((value << 1) | valueMSbit);

    SetFlags(result, registerBank, valueMSbit);
    WriteResult(result, registerBank, decodedInstruction);
}

inline void InstructionRlc::DecodeRlcRegisterMode(uint8_t opcode, interfaces::DecodedInstruction& decodedInstruction)
{
    auto source = RegisterBankInterface::FromInstructionSource(opcode);
    decodedInstruction =
    {
        .Opcode = OpcodeType::rlc,
        .AddressingMode = AddressingMode::Register,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = source,
        .DestinationRegister = source,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0x00
    };
}

inline void InstructionRlc::DecodeRlcRegisterIndirectMode(interfaces::DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::rlc,
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

inline void InstructionRlc::WriteResult(uint8_t result, RegisterBankInterface* registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.AddressingMode == AddressingMode::Register)
        registerBank->Write(decodedInstruction.DestinationRegister, result);
    else
        decodedInstruction.MemoryResult1 = result;
}

inline uint8_t InstructionRlc::AcquireOperand(RegisterBankInterface* registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.AddressingMode == AddressingMode::Register)
        return registerBank->Read(decodedInstruction.SourceRegister);
    else
        return decodedInstruction.MemoryOperand1;
}

inline void InstructionRlc::SetFlags(uint8_t result, RegisterBankInterface* registerBank, uint8_t flagValue)
{
    registerBank->WriteFlag(Flag::CY, flagValue);
    registerBank->WriteFlag(Flag::Z, (result == 0? 0x01 : 0x00));
    registerBank->ClearFlag(Flag::H);
    registerBank->ClearFlag(Flag::N);
}

}