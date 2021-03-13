#include "InstructionPush.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::instructions
{

void InstructionPush::Decode(uint8_t opcode, [[maybe_unused]] std::optional<uint8_t> preOpcode, interfaces::DecodedInstruction& decodedInstruction)
{
    auto source = RegisterBankInterface::FromInstructionToPushPair((opcode >> 4) & 0x03);
    decodedInstruction =
    {
        .Opcode = OpcodeType::push,
        .AddressingMode = AddressingMode::RegisterIndirectDestinationPair,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = source,
        .DestinationRegister = Register::SP,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

void InstructionPush::Execute(std::shared_ptr<interfaces::RegisterBankInterface> registerBank, interfaces::DecodedInstruction& decodedInstruction)
{
    auto sourceValue = registerBank->ReadPair(decodedInstruction.SourceRegister);
    decodedInstruction.MemoryResult1 = (sourceValue >> 0x08) & 0xFF;
    decodedInstruction.MemoryResult2 = (sourceValue) & 0xFF;
}

}