#include "InstructionLdhl.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::instructions
{

void InstructionLdhl::Decode([[maybe_unused]] uint8_t opcode, [[maybe_unused]] std::optional<uint8_t> preOpcode, interfaces::DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::ldhl,
        .AddressingMode = AddressingMode::Immediate,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::SP, 
        .DestinationRegister = Register::HL,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0x00
    };
}

void InstructionLdhl::Execute(RegisterBankInterface* registerBank, interfaces::DecodedInstruction& decodedInstruction)
{   
    registerBank->Write(Register::F, 0x00);

    auto operand1 = registerBank->ReadPair(decodedInstruction.SourceRegister);
    auto operand2 = static_cast<uint16_t>((decodedInstruction.MemoryOperand1 >> 7 != 0? 0xFF00 : 0x0000) | decodedInstruction.MemoryOperand1);

    auto result = static_cast<uint16_t>(0x00);
    auto carryIn = 0;

    for (auto i = 0; i < 16; i++)
    {
        auto operand1Bit = (operand1 >> i) & 0x01;
        auto operand2Bit = (operand2 >> i) & 0x01;
        auto resultBit = (operand1Bit ^ operand2Bit) ^ carryIn;
        carryIn = (operand1Bit & operand2Bit) | (carryIn & (operand1Bit ^ operand2Bit));

        if (i == 11 && carryIn)
            registerBank->SetFlag(Flag::H);
        else if (i == 15 && carryIn)
            registerBank->SetFlag(Flag::CY);

        result |= (resultBit << i);
    }

    registerBank->ClearFlag(Flag::Z);
    registerBank->ClearFlag(Flag::N);

    registerBank->WritePair(decodedInstruction.DestinationRegister, result);
}

}