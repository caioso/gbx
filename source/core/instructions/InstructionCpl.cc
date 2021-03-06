#include "InstructionCpl.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::instructions
{

void InstructionCpl::Decode([[maybe_unused]] uint8_t opcode, [[maybe_unused]] optional<uint8_t> preOpcode, DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::cpl,
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

void InstructionCpl::Execute(RegisterBankInterface* registerBank, DecodedInstruction& decodedInstruction)
{
    auto sourceValue = registerBank->Read(decodedInstruction.SourceRegister);
    registerBank->Write(decodedInstruction.DestinationRegister, ~sourceValue);

    registerBank->SetFlag(Flag::H);
    registerBank->SetFlag(Flag::N);
}

}