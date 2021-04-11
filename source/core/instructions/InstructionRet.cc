#include "InstructionRet.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::instructions
{

void InstructionRet::Decode(uint8_t opcode, optional<uint8_t> preOpcode, DecodedInstruction& decodedInstruction)
{
    if (opcode == 0xC9)
        DecodeUnconditionalRet(opcode, preOpcode, decodedInstruction);
    else
        DecodeConditionalRet(opcode, preOpcode, decodedInstruction);
}

void InstructionRet::Execute(RegisterBankInterface* registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.InstructionExtraOperand == 0xFF)
        ExecuteUnconditionalRet(registerBank, decodedInstruction);
    else
        ExecuteConditionalRet(registerBank, decodedInstruction);
}

inline void InstructionRet::ExecuteConditionalRet(RegisterBankInterface* registerBank, DecodedInstruction& decodedInstruction)
{
    auto condition = decodedInstruction.InstructionExtraOperand;
    auto zFlag = registerBank->ReadFlag(Flag::Z);
    auto cyFlag = registerBank->ReadFlag(Flag::CY);

    if ((condition == 0x00 && zFlag == 0x00) || (condition == 0x01 && zFlag == 0x01) ||
        (condition == 0x02 && cyFlag == 0x00) || (condition == 0x03 && cyFlag == 0x01))
        ExecuteUnconditionalRet(registerBank, decodedInstruction);
    else
    {
        auto currentSp = registerBank->ReadPair(Register::SP);
        currentSp -= 2;
        registerBank->WritePair(Register::SP, currentSp);
    }
}

inline void InstructionRet::ExecuteUnconditionalRet(RegisterBankInterface* registerBank, DecodedInstruction& decodedInstruction)
{
    auto newPCAddress = static_cast<uint16_t>(decodedInstruction.MemoryOperand1 | (decodedInstruction.MemoryOperand2 << 8));
    registerBank->WritePair(decodedInstruction.DestinationRegister, newPCAddress);
}

inline void InstructionRet::DecodeConditionalRet([[maybe_unused]] uint8_t opcode, [[maybe_unused]]  optional<uint8_t> preOpcode, DecodedInstruction& decodedInstruction)
{
    auto conditionalFlag = static_cast<uint8_t>((opcode >> 0x03) & 0x03);

    decodedInstruction =
    {
        .Opcode = OpcodeType::ret,
        .AddressingMode = AddressingMode::RegisterIndirectSourcePair,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::SP,
        .DestinationRegister = Register::PC,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = conditionalFlag
    };
}

inline void InstructionRet::DecodeUnconditionalRet([[maybe_unused]] uint8_t opcode, [[maybe_unused]]  optional<uint8_t> preOpcode, DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::ret,
        .AddressingMode = AddressingMode::RegisterIndirectSourcePair,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::SP,
        .DestinationRegister = Register::PC,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0xFF
    };
}

}