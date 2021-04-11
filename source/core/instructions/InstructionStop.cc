#include "InstructionStop.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::instructions
{

void InstructionStop::Decode([[maybe_unused]] uint8_t opcode, [[maybe_unused]] optional<uint8_t> preOpcode, DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::stop,
        .AddressingMode = AddressingMode::Immediate,
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

void InstructionStop::Execute([[maybe_unused]] RegisterBankInterface* registerBank, [[maybe_unused]] DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.MemoryOperand1 != 0x00)
        throw InstructionException("Invalid 'STOP' instruction operand");
}

}