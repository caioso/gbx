#include "InstructionPop.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::instructions
{

void InstructionPop::Decode(uint8_t opcode, [[maybe_unused]] std::optional<uint8_t> preOpcode, interfaces::DecodedInstruction& decodedInstruction)
{
    auto destination = RegisterBankInterface::FromInstructionToPushPair((opcode >> 4) & 0x03);
    decodedInstruction =
    {
        .Opcode = OpcodeType::pop,
        .AddressingMode = AddressingMode::RegisterIndirectSourcePair,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::SP,
        .DestinationRegister = destination,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0x00
    };
}

void InstructionPop::Execute(std::shared_ptr<interfaces::RegisterBankInterface> registerBank, interfaces::DecodedInstruction& decodedInstruction)
{
    auto value = static_cast<uint16_t>(decodedInstruction.MemoryOperand1 | decodedInstruction.MemoryOperand2 << 0x08);
    registerBank->WritePair(decodedInstruction.DestinationRegister, value);
}

}