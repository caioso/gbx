#include "InstructionEi.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx
{

void InstructionEi::Decode(__attribute__((unused)) uint8_t opcode, __attribute__((unused)) optional<uint8_t> preOpcode, DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::ei,
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

void InstructionEi::Execute(__attribute__((unused)) shared_ptr<RegisterBankInterface> registerBank, __attribute__((unused)) DecodedInstruction& decodedInstruction)
{
    return;

}
}