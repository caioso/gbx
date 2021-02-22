#include "InstructionSrl.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::instructions
{

void InstructionSrl::Decode(uint8_t opcode, __attribute__((unused)) optional<uint8_t> preOpcode, DecodedInstruction&decodedInstruction)
 {
    if ((opcode & 0x07) == 0x06) // Slr (HL)
        DecodeSrlRegisterIndirectMode(decodedInstruction);
    else // Slr r
        DecodeSrlRegisterMode(opcode, decodedInstruction);
}

void InstructionSrl::Execute(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    auto operandValue = AcquireOperand(registerBank, decodedInstruction);
    auto lSBit = static_cast<uint8_t>(operandValue & 0x01);
    auto result = static_cast<uint8_t>(operandValue >> 1);

    SetFlags(result, registerBank, lSBit);
    WriteResult(result, registerBank, decodedInstruction);
}

inline void InstructionSrl::WriteResult(uint8_t result, shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.AddressingMode == AddressingMode::Register)
        registerBank->Write(decodedInstruction.DestinationRegister, result);
    else
        decodedInstruction.MemoryResult1 = result;
}

inline uint8_t InstructionSrl::AcquireOperand(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.AddressingMode == AddressingMode::Register)
        return registerBank->Read(decodedInstruction.SourceRegister);
    else
        return decodedInstruction.MemoryOperand1;
}

inline void InstructionSrl::DecodeSrlRegisterMode(uint8_t opcode, interfaces::DecodedInstruction& decodedInstruction)
{
    auto operand = RegisterBankInterface::FromInstructionSource((opcode & 0x7));
    decodedInstruction =
    {
        .Opcode = OpcodeType::srl,
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

inline void InstructionSrl::DecodeSrlRegisterIndirectMode(interfaces::DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::srl,
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

inline void InstructionSrl::SetFlags(uint8_t result, shared_ptr<RegisterBankInterface> registerBank, uint8_t flagValue)
{
    registerBank->WriteFlag(Flag::CY, flagValue);
    registerBank->WriteFlag(Flag::Z, (result == 0? 0x01 : 0x00));
    registerBank->ClearFlag(Flag::H);
    registerBank->ClearFlag(Flag::N);
}

}