#include "InstructionJr.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::instructions
{

void InstructionJr::Decode(uint8_t opcode, __attribute__((unused)) std::optional<uint8_t> preOpcode, interfaces::DecodedInstruction& decodedInstruction)
{
    if ((opcode & 0x20) == 0x00)
        DecodeUnconditionalJr(decodedInstruction);    
    else
        DecodeConditionalJr(opcode, decodedInstruction);
}

void InstructionJr::Execute(std::shared_ptr<interfaces::RegisterBankInterface> registerBank, interfaces::DecodedInstruction& decodedInstruction) 
{
    if (decodedInstruction.InstructionExtraOperand == 0xFF)
        ExecuteUnconditionalJr(registerBank, decodedInstruction);
    else
        ExecuteConditionalJr(registerBank, decodedInstruction);
}

inline void InstructionJr::ExecuteUnconditionalJr(std::shared_ptr<interfaces::RegisterBankInterface> registerBank, interfaces::DecodedInstruction& decodedInstruction)
{
    auto offset = static_cast<int8_t>(decodedInstruction.MemoryOperand1);
    auto targetPCAddress = static_cast<uint16_t>(registerBank->ReadPair(Register::PC) + (offset));
    registerBank->WritePair(Register::PC, targetPCAddress);
}

inline void InstructionJr::ExecuteConditionalJr(std::shared_ptr<interfaces::RegisterBankInterface> registerBank, interfaces::DecodedInstruction& decodedInstruction)
{
    auto condition = decodedInstruction.InstructionExtraOperand;
    auto zFlag = registerBank->ReadFlag(Flag::Z);
    auto cyFlag = registerBank->ReadFlag(Flag::CY);

    if ((condition == 0x00 && zFlag == 0x00) || (condition == 0x01 && zFlag == 0x01) ||
        (condition == 0x02 && cyFlag == 0x00) || (condition == 0x03 && cyFlag == 0x01))
        ExecuteUnconditionalJr(registerBank, decodedInstruction);
}

inline void InstructionJr::DecodeUnconditionalJr(interfaces::DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::jr,
        .AddressingMode = AddressingMode::Immediate,
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

inline void InstructionJr::DecodeConditionalJr(uint8_t opcode, interfaces::DecodedInstruction& decodedInstruction)
{
    auto conditionalFlag = static_cast<uint8_t>((opcode >> 0x03) & 0x03);

    decodedInstruction =
    {
        .Opcode = OpcodeType::jr,
        .AddressingMode = AddressingMode::Immediate,
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