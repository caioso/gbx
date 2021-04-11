#include "InstructionScf.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::instructions
{

void InstructionScf::Decode([[maybe_unused]] uint8_t opcode, [[maybe_unused]] optional<uint8_t> preOpcode, DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::scf,
        .AddressingMode = AddressingMode::Register,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::NoRegister,
        .DestinationRegister = Register::NoRegister,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0x00
    };
}

void InstructionScf::Execute([[maybe_unused]] RegisterBankInterface* registerBank, [[maybe_unused]] DecodedInstruction& decodedInstruction)
{
    registerBank->WriteFlag(Flag::CY, 1);
    registerBank->WriteFlag(Flag::H, 0);
    registerBank->WriteFlag(Flag::N, 0);
}

}