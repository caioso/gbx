#include "InstructionReti.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx
{

void InstructionReti::Decode(__attribute__((unused)) uint8_t opcode, __attribute__((unused)) optional<uint8_t> preOpcode, DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::reti,
        .AddressingMode = AddressingMode::RegisterIndirectSourcePair,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::SP,
        .DestinationRegister = Register::PC,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0xFF
    };
}

void InstructionReti::Execute(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    auto newPCAddress = static_cast<uint16_t>(decodedInstruction.MemoryOperand1 | (decodedInstruction.MemoryOperand2 << 8));
    registerBank->WritePair(decodedInstruction.DestinationRegister, newPCAddress);
}

}