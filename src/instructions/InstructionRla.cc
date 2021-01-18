#include "InstructionRla.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx
{

void InstructionRla::Decode(__attribute__((unused)) uint8_t opcode, __attribute__((unused)) optional<uint8_t> preOpcode, DecodedInstruction&decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::rla,
        .AddressingMode = AddressingMode::Register,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::A,
        .DestinationRegister = Register::A,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };    
}

void InstructionRla::Execute(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    auto value = registerBank->Read(decodedInstruction.SourceRegister);
    auto currentCarry = registerBank->ReadFlag(Flag::CY);
    auto valueMSbit = static_cast<uint8_t>((value >> 7) & 0x01);
    auto result = static_cast<uint8_t>((value << 1) | currentCarry);

    SetFlags(registerBank, valueMSbit);
    registerBank->Write(decodedInstruction.DestinationRegister, result);
}

inline void InstructionRla::SetFlags(shared_ptr<RegisterBankInterface> registerBank, uint8_t flagValue)
{
    registerBank->WriteFlag(Flag::CY, flagValue);
    registerBank->ClearFlag(Flag::Z);
    registerBank->ClearFlag(Flag::H);
    registerBank->ClearFlag(Flag::N);
}

}