#include "InstructionRlc.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx
{

void InstructionRlc::Decode( __attribute__((unused)) uint8_t opcode, optional<uint8_t> complement, DecodedInstruction&decodedInstruction)
{
    if (!complement.has_value())
        throw InstructionException("Rlc requires a complement");

    auto source = RegisterBankInterface::FromInstructionSource(complement.value());
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
        .MemoryResult2 = 0x00
    };    
}

void InstructionRlc::Execute(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    auto value = registerBank->Read(decodedInstruction.SourceRegister);
    auto valueMSbit = static_cast<uint8_t>((value >> 7) & 0x01);
    auto result = static_cast<uint8_t>((value << 1) | valueMSbit);

    SetFlags(registerBank, valueMSbit);
    registerBank->Write(decodedInstruction.DestinationRegister, result);
}

inline void InstructionRlc::SetFlags(shared_ptr<RegisterBankInterface> registerBank, uint8_t flagValue)
{
    registerBank->WriteFlag(Flag::CY, flagValue);
    registerBank->ClearFlag(Flag::H);
    registerBank->ClearFlag(Flag::N);
}

}