#include "InstructionDec.h"

using namespace std;
using namespace gbx::interfaces;

namespace gbx
{
void InstructionDec::Decode(uint8_t opcode, __attribute__((unused)) std::optional<uint8_t> preOpcode, DecodedInstruction& decodedInstruction)
{
    if (opcode == 0x35)
        DecodeDecRegisterIndirectMode(decodedInstruction);
    else if ((opcode >> 0x06) == 0x00 && (opcode & 0x07) == 0x05)
        DecodeDecRegisterMode(opcode, decodedInstruction);
    
}

void InstructionDec::Execute(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction) 
{
    auto operandValue = AcquireSourceOperandValue(registerBank, decodedInstruction);
    auto carry = registerBank->ReadFlag(Flag::CY);

    registerBank->Write(Register::F, 0x00);
    SetDestinationOperandValue(operandValue, registerBank, decodedInstruction);

    if (carry)
        registerBank->SetFlag(Flag::CY);
    else
        registerBank->ClearFlag(Flag::CY);    
}

inline void InstructionDec::SetDestinationOperandValue(uint8_t operandValue, shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    if (auto value = CalculateBinarySubtractionAndSetFlags(operandValue, 0x01, nullopt, registerBank);
        decodedInstruction.AddressingMode == AddressingMode::Register)
        registerBank->Write(decodedInstruction.DestinationRegister, value);
    else 
        decodedInstruction.MemoryResult1 = value;
}

inline void InstructionDec::DecodeDecRegisterMode(uint8_t opcode, DecodedInstruction& decodedInstruction)
{
    auto source = RegisterBankInterface::FromInstructionSource((opcode >> 0x03) & 0x07);
    decodedInstruction =
    {
        .Opcode = OpcodeType::dec,
        .AddressingMode = AddressingMode::Register,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = source,
        .DestinationRegister = source,
        .MemoryResult1 = 0x00
    };
}

inline void InstructionDec::DecodeDecRegisterIndirectMode(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::dec,
        .AddressingMode = AddressingMode::RegisterIndirectSourceAndDestination,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::HL,
        .DestinationRegister = Register::HL,
        .MemoryResult1 = 0x00
    };
}

}