#include "InstructionRst.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx
{

void InstructionRst::Decode(uint8_t opcode, __attribute__((opcode)) optional<uint8_t>, DecodedInstruction& decodedInstruction) 
{
    auto pageCode = static_cast<uint8_t>((opcode >> 0x03) & 0x07);

    decodedInstruction =
    {
        .Opcode = OpcodeType::rst,
        .AddressingMode = AddressingMode::RegisterIndirectSourcePair,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::SP,
        .DestinationRegister = Register::PC,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = pageCode
    };
}

void InstructionRst::Execute(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{

}

}
