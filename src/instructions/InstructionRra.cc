#include "InstructionRra.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx
{

void InstructionRra::Decode(__attribute__((unused)) uint8_t opcode, __attribute__((unused)) optional<uint8_t> preOpcode, DecodedInstruction&decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::rra,
        .AddressingMode = AddressingMode::Register,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::A,
        .DestinationRegister = Register::A,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0x00
    };    
}

void InstructionRra::Execute(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction, __attribute__((unused)) bool& isWriteBackAborted)
{
    auto value = registerBank->Read(decodedInstruction.SourceRegister);
    auto currentCarry = registerBank->ReadFlag(Flag::CY);
    auto lSBit = static_cast<uint8_t>(value & 0x01);
    auto result = static_cast<uint8_t>((value >> 1) | (currentCarry << 7));

    SetFlags(registerBank, lSBit);
    registerBank->Write(decodedInstruction.DestinationRegister, result);
}

inline void InstructionRra::SetFlags(shared_ptr<RegisterBankInterface> registerBank, uint8_t flagValue)
{
    registerBank->WriteFlag(Flag::CY, flagValue);
    registerBank->ClearFlag(Flag::Z);
    registerBank->ClearFlag(Flag::H);
    registerBank->ClearFlag(Flag::N);
}

}