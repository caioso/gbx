#include "InstructionCp.h"

using namespace std;
using namespace gbx::interfaces;

namespace gbx
{
void InstructionCp::Decode(uint8_t opcode, __attribute__((unused)) std::optional<uint8_t> preOpcode, DecodedInstruction& decodedInstruction)
{
    if (opcode == 0xFE)
       DecodeCpImmediateMode(decodedInstruction);
    else if (opcode == 0xBE)
       DecodeCpRegisterIndirectMode(decodedInstruction);
    else if ((opcode >> 0x03) == 0x17)
        DecodeCpRegisterMode(opcode, decodedInstruction);
}

void InstructionCp::Execute(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction) 
{
    auto operand1 = registerBank->Read(decodedInstruction.DestinationRegister);
    auto operand2 = GetSourceOperandValue(registerBank, decodedInstruction);
    CalculateDifferenceAndSetFlags(operand1, operand2, registerBank);
}

void InstructionCp::CalculateDifferenceAndSetFlags(uint8_t operand1, uint8_t operand2, shared_ptr<RegisterBankInterface> registerBank)
{
    auto result = static_cast<uint8_t>(0x00);
    auto borrowIn = static_cast<uint8_t>(0x00);

    registerBank->Write(Register::F, 0x00);

    for (auto i = 0; i < 8; i++)
    {
        auto aBit = (operand1 >> i) & 0x01;
        auto operandBit = (operand2 >> i) & 0x01;

        auto resultBit = (aBit ^ operandBit) ^ borrowIn;
        borrowIn = (~(aBit) & operandBit) | ((~(aBit ^ operandBit)) & borrowIn);

        if (i == 3 && borrowIn == 0x01)
            registerBank->SetFlag(Flag::H);
        else if (i == 7 && borrowIn == 0x01)
            registerBank->SetFlag(Flag::CY);

        result |= (resultBit << i);
    }

    if (result == 0x00)
        registerBank->SetFlag(Flag::Z);

    registerBank->SetFlag(Flag::N);
}

inline uint8_t InstructionCp::GetSourceOperandValue(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.AddressingMode == AddressingMode::Register)
        return registerBank->Read(decodedInstruction.SourceRegister);
    else 
        return decodedInstruction.MemoryOperand1;
}

inline void InstructionCp::DecodeCpRegisterMode(uint8_t opcode, DecodedInstruction& decodedInstruction)
{
    auto source = RegisterBankInterface::FromInstructionSource(opcode & 0x07);
    decodedInstruction =
    {
        .Opcode = OpcodeType::cp,
        .AddressingMode = AddressingMode::Register,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = source,
        .DestinationRegister = Register::A,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionCp::DecodeCpImmediateMode(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::cp,
        .AddressingMode = AddressingMode::Immediate,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::NoRegiser,
        .DestinationRegister = Register::A,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionCp::DecodeCpRegisterIndirectMode(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::cp,
        .AddressingMode = AddressingMode::RegisterIndirectSource,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::HL,
        .DestinationRegister = Register::A,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

}