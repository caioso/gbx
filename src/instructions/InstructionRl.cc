#include "InstructionRl.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx
{

void InstructionRl::Decode(uint8_t opcode, __attribute__((unused)) optional<uint8_t> complement, DecodedInstruction&decodedInstruction)
{
    if (opcode == 0x16) // RL (HL)
        DecodeRlRegisterIndirectMode(decodedInstruction);
    else // RL r
        DecodeRlRegisterMode(opcode, decodedInstruction);
}

void InstructionRl::Execute(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    auto value = AcquireOperand(registerBank, decodedInstruction);

    if (decodedInstruction.Opcode == OpcodeType::rlc)
    {
        auto valueMSbit = static_cast<uint8_t>((value >> 7) & 0x01);
        auto result = static_cast<uint8_t>((value << 1) | valueMSbit);

        SetFlags(registerBank, valueMSbit);
        WriteResult(result, registerBank, decodedInstruction);
    }
    else
    {
        auto valueMSbit = static_cast<uint8_t>((value >> 7) & 0x01);
        auto carry = registerBank->ReadFlag(Flag::CY);
        auto result = static_cast<uint8_t>((value << 1) | carry);

        SetFlags(registerBank, valueMSbit);
        WriteResult(result, registerBank, decodedInstruction);
    }
}

inline void InstructionRl::DecodeRlRegisterMode(uint8_t complement, interfaces::DecodedInstruction& decodedInstruction)
{
    auto source = RegisterBankInterface::FromInstructionSource(complement & 0x07);
    decodedInstruction =
    {
        .Opcode = OpcodeType::rl,
        .AddressingMode = AddressingMode::Register,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = source,
        .DestinationRegister = source,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionRl::DecodeRlRegisterIndirectMode(interfaces::DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::rl,
        .AddressingMode = AddressingMode::RegisterIndirectSourceAndDestination,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::HL,
        .DestinationRegister = Register::HL,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionRl::WriteResult(uint8_t result, shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.AddressingMode == AddressingMode::Register)
        registerBank->Write(decodedInstruction.DestinationRegister, result);
    else
        decodedInstruction.MemoryResult1 = result;
}

inline uint8_t InstructionRl::AcquireOperand(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.AddressingMode == AddressingMode::Register)
        return registerBank->Read(decodedInstruction.SourceRegister);
    else
        return decodedInstruction.MemoryOperand1;
}

inline void InstructionRl::SetFlags(shared_ptr<RegisterBankInterface> registerBank, uint8_t flagValue)
{
    registerBank->WriteFlag(Flag::CY, flagValue);
    registerBank->ClearFlag(Flag::H);
    registerBank->ClearFlag(Flag::N);
}

}