#include "InstructionRr.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::instructions
{

void InstructionRr::Decode(uint8_t opcode, [[maybe_unused]] optional<uint8_t> preOpcode, DecodedInstruction&decodedInstruction)
{
    if ((opcode & 0x07) == 0x06) // RR (HL)
        DecodeRrRegisterIndirectMode(decodedInstruction);
    else // RR r
        DecodeRrRegisterMode(opcode, decodedInstruction);
}

void InstructionRr::Execute(RegisterBankInterface* registerBank, DecodedInstruction& decodedInstruction)
{
    auto operandValue = AcquireOperand(registerBank, decodedInstruction);
    auto carry = registerBank->ReadFlag(Flag::CY);
    auto lsBit = static_cast<uint8_t>(operandValue & 0x01);
    auto result = static_cast<uint8_t>((operandValue >> 1) | (carry << 7));

    SetFlags(result, registerBank, lsBit);
    WriteResult(result, registerBank, decodedInstruction);
}

inline void InstructionRr::WriteResult(uint8_t result, RegisterBankInterface* registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.AddressingMode == AddressingMode::Register)
        registerBank->Write(decodedInstruction.DestinationRegister, result);
    else
        decodedInstruction.MemoryResult1 = result;
}

inline uint8_t InstructionRr::AcquireOperand(RegisterBankInterface* registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.AddressingMode == AddressingMode::Register)
        return registerBank->Read(decodedInstruction.SourceRegister);
    else
        return decodedInstruction.MemoryOperand1;
}

inline void InstructionRr::DecodeRrRegisterMode(uint8_t opcode, interfaces::DecodedInstruction& decodedInstruction)
{
    auto operand = RegisterBankInterface::FromInstructionSource((opcode & 0x7));
    decodedInstruction =
    {
        .Opcode = OpcodeType::rr,
        .AddressingMode = AddressingMode::Register,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = operand,
        .DestinationRegister = operand,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0x00
    };    
}

inline void InstructionRr::DecodeRrRegisterIndirectMode(interfaces::DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::rr,
        .AddressingMode = AddressingMode::RegisterIndirectSourceAndDestination,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::HL,
        .DestinationRegister = Register::HL,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0x00
    };
}

inline void InstructionRr::SetFlags(uint8_t result, RegisterBankInterface* registerBank, uint8_t flagValue)
{
    registerBank->WriteFlag(Flag::CY, flagValue);
    registerBank->WriteFlag(Flag::Z, (result == 0? 0x01 : 0x00));
    registerBank->ClearFlag(Flag::H);
    registerBank->ClearFlag(Flag::N);
}

}