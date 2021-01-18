#include "InstructionRlc.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx
{

void InstructionRlc::Decode(__attribute__((unused)) uint8_t opcode, __attribute__((unused)) optional<uint8_t> preOpcode, DecodedInstruction&decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::rlc,
        .AddressingMode = AddressingMode::Immediate,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::NoRegiser,
        .DestinationRegister = Register::NoRegiser,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };    
}

void InstructionRlc::Execute(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    auto targetRegister = RegisterBankInterface::FromInstructionSource(decodedInstruction.MemoryOperand1);
    auto value = registerBank->Read(targetRegister);
    auto valueMSbit = static_cast<uint8_t>((value >> 7) & 0x01);
    auto result = static_cast<uint8_t>((value << 1) | valueMSbit);

    SetFlags(registerBank, valueMSbit);
    registerBank->Write(targetRegister, result);
}

inline void InstructionRlc::SetFlags(shared_ptr<RegisterBankInterface> registerBank, uint8_t flagValue)
{
    registerBank->WriteFlag(Flag::CY, flagValue);
    registerBank->ClearFlag(Flag::H);
    registerBank->ClearFlag(Flag::N);
}

}