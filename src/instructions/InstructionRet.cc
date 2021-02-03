#include "InstructionRet.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx
{

void InstructionRet::Decode(uint8_t opcode, optional<uint8_t> preOpcode, DecodedInstruction& decodedInstruction)
{
    if (opcode == 0xC9)
        DecodeUnconditionalRet(opcode, preOpcode, decodedInstruction);
    else
        DecodeConditionalRet(opcode, preOpcode, decodedInstruction);
}

void InstructionRet::Execute(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction, __attribute__((unused)) bool& isWriteBackAborted)
{
    if (decodedInstruction.InstructionExtraOperand == 0xFF)
        ExecuteUnconditionalRet(registerBank, decodedInstruction, isWriteBackAborted);
    else
        ExecuteConditionalRet(registerBank, decodedInstruction, isWriteBackAborted);
}

inline void InstructionRet::ExecuteConditionalRet(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction, __attribute__((unused)) bool& isWriteBackAborted)
{
    auto condition = decodedInstruction.InstructionExtraOperand;
    auto zFlag = registerBank->ReadFlag(Flag::Z);
    auto cyFlag = registerBank->ReadFlag(Flag::CY);

    if ((condition == 0x00 && zFlag == 0x00) || (condition == 0x01 && zFlag == 0x01) ||
        (condition == 0x02 && cyFlag == 0x00) || (condition == 0x03 && cyFlag == 0x01))
        ExecuteUnconditionalRet(registerBank, decodedInstruction, isWriteBackAborted);
    else
    {
        auto currentSp = registerBank->ReadPair(Register::SP);
        currentSp -= 2;
        registerBank->WritePair(Register::SP, currentSp);
    }
}

inline void InstructionRet::ExecuteUnconditionalRet(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction, __attribute__((unused)) bool& isWriteBackAborted)
{
    auto newPCAddress = static_cast<uint16_t>(decodedInstruction.MemoryOperand1 | (decodedInstruction.MemoryOperand2 << 8));
    registerBank->WritePair(decodedInstruction.DestinationRegister, newPCAddress);
}

inline void InstructionRet::DecodeConditionalRet(__attribute__((unused)) uint8_t opcode, __attribute__((unused))  optional<uint8_t> preOpcode, DecodedInstruction& decodedInstruction)
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

inline void InstructionRet::DecodeUnconditionalRet(__attribute__((unused)) uint8_t opcode, __attribute__((unused))  optional<uint8_t> preOpcode, DecodedInstruction& decodedInstruction)
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