#include "InstructionRst.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::instructions
{

void InstructionRst::Decode(uint8_t opcode, __attribute__((unused)) optional<uint8_t> preOpcode, DecodedInstruction& decodedInstruction) 
{
    auto pageCode = static_cast<uint8_t>((opcode >> 0x03) & 0x07);

    decodedInstruction =
    {
        .Opcode = OpcodeType::rst,
        .AddressingMode = AddressingMode::RegisterIndirectDestinationPair,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::PC,
        .DestinationRegister = Register::SP,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = pageCode
    };
}

void InstructionRst::Execute(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    auto sourceValue = registerBank->ReadPair(decodedInstruction.SourceRegister);
    auto lowerByte = GetPageLowerAddress(decodedInstruction.InstructionExtraOperand);
    auto newPCAddress = lowerByte;
    
    registerBank->WritePair(Register::PC, newPCAddress);

    decodedInstruction.MemoryResult1 = static_cast<uint8_t>((sourceValue >> 8) & 0xFF);
    decodedInstruction.MemoryResult2 = static_cast<uint8_t>(sourceValue & 0xFF);
}

uint8_t InstructionRst::GetPageLowerAddress(uint8_t page)
{
    switch (page)
    {
        case 0x00: return 0x00;
        case 0x01: return 0x08;
        case 0x02: return 0x10;
        case 0x03: return 0x18;
        case 0x04: return 0x20;
        case 0x05: return 0x28;
        case 0x06: return 0x30;
        case 0x07: return 0x38;
    }
    return 0x00;
}

}
