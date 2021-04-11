#include "InstructionJp.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::instructions
{

void InstructionJp::Decode([[maybe_unused]] uint8_t opcode, [[maybe_unused]] std::optional<uint8_t> preOpcode, interfaces::DecodedInstruction& decodedInstruction)
{
    if (opcode == 0xE9)
        DecodeUnconditionalJpRegisterIndirect(decodedInstruction);
    else if ((opcode & 0x03) == 0x03)
        DecodeUnconditionalJpImmediate(decodedInstruction);
    else if ((opcode & 0x03) == 0x02)
        DecodeConditionalJp(opcode, decodedInstruction);
}

void InstructionJp::Execute(RegisterBankInterface* registerBank, interfaces::DecodedInstruction& decodedInstruction) 
{
    // Flag to indicate which jump typ it is
    if (decodedInstruction.InstructionExtraOperand == 0xFF)
        ExecuteUnconditionalJp(registerBank, decodedInstruction);
    else if (decodedInstruction.InstructionExtraOperand == 0xFE)
        ExecuteUnconditionalJpRegisterIndirect(registerBank, decodedInstruction);
    else
        ExecuteConditionalJp(registerBank, decodedInstruction);
}

void InstructionJp::ExecuteConditionalJp(RegisterBankInterface* registerBank, interfaces::DecodedInstruction& decodedInstruction) 
{
    auto condition = decodedInstruction.InstructionExtraOperand;
    auto zFlag = registerBank->ReadFlag(Flag::Z);
    auto cyFlag = registerBank->ReadFlag(Flag::CY);

    if ((condition == 0x00 && zFlag == 0x00) || (condition == 0x01 && zFlag == 0x01) ||
        (condition == 0x02 && cyFlag == 0x00) || (condition == 0x03 && cyFlag == 0x01))
        ExecuteUnconditionalJp(registerBank, decodedInstruction);
}

void InstructionJp::ExecuteUnconditionalJp(RegisterBankInterface* registerBank, interfaces::DecodedInstruction& decodedInstruction) 
{
    auto lsByte = decodedInstruction.MemoryOperand1;
    auto msByte = decodedInstruction.MemoryOperand2;
    registerBank->WritePair(Register::PC, lsByte | (msByte << 0x08));
}

void InstructionJp::ExecuteUnconditionalJpRegisterIndirect(RegisterBankInterface* registerBank, interfaces::DecodedInstruction& decodedInstruction) 
{
    auto lsByte = decodedInstruction.MemoryResult1;
    auto msByte = decodedInstruction.MemoryResult2;
    registerBank->WritePair(Register::PC, lsByte | (msByte << 0x08));
}

void InstructionJp::DecodeUnconditionalJpImmediate(interfaces::DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::jp,
        .AddressingMode = AddressingMode::ImmediatePair,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::NoRegister,
        .DestinationRegister = Register::PC,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0xFF
    };    
}

void InstructionJp::DecodeUnconditionalJpRegisterIndirect(interfaces::DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::jp,
        .AddressingMode = AddressingMode::RegisterIndirectSourcePair,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::HL,
        .DestinationRegister = Register::PC,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0xFF
    };    
}

void InstructionJp::DecodeConditionalJp(uint8_t opcode, interfaces::DecodedInstruction& decodedInstruction)
{
    auto conditionalFlag = static_cast<uint8_t>((opcode >> 0x03) & 0x03);

    decodedInstruction =
    {
        .Opcode = OpcodeType::jp,
        .AddressingMode = AddressingMode::ImmediatePair,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::NoRegister,
        .DestinationRegister = Register::PC,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = conditionalFlag
    };    
}

}