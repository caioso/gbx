#include "InstructionRlca.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx
{

void InstructionRlca::Decode(__attribute__((unused)) uint8_t opcode, __attribute__((unused)) std::optional<uint8_t> preOpcode, interfaces::DecodedInstruction&decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::rlca,
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

void InstructionRlca::Execute(std::shared_ptr<interfaces::RegisterBankInterface> registerBank, interfaces::DecodedInstruction& decodedInstruction)
{
    auto value = registerBank->Read(decodedInstruction.SourceRegister);
    auto valueMSbit = static_cast<uint8_t>((value >> 7) & 0x01);
    auto result = static_cast<uint8_t>((value << 1) | valueMSbit);

    SetFlags(registerBank, valueMSbit);
    registerBank->Write(decodedInstruction.DestinationRegister, result);
}

inline void InstructionRlca::SetFlags(shared_ptr<RegisterBankInterface> registerBank, uint8_t flagValue)
{
    registerBank->WriteFlag(Flag::CY, flagValue);
    registerBank->ClearFlag(Flag::Z);
    registerBank->ClearFlag(Flag::H);
    registerBank->ClearFlag(Flag::N);
}

}