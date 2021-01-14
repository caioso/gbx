#include "InstructionPop.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx
{

void InstructionPop::Decode(uint8_t opcode, __attribute__((unused)) std::optional<uint8_t> preOpcode, interfaces::DecodedInstruction& decodedInstruction)
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
        .MemoryResult2 = 0x00
    };
}

void InstructionPop::Execute(std::shared_ptr<interfaces::RegisterBankInterface> registerBank, interfaces::DecodedInstruction& decodedInstruction)
{
    auto value = static_cast<uint16_t>(decodedInstruction.MemoryOperand1 << 0x08 | decodedInstruction.MemoryOperand2);
    registerBank->WritePair(decodedInstruction.DestinationRegister, value);
}

}