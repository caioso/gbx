#include "InstructionSbc.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx
{

void InstructionSbc::Decode(uint8_t opcode, __attribute__((unused)) optional<uint8_t> preOpcode, DecodedInstruction& decodedInstruction)
{
    if (opcode == 0xDE) // SBC A, #XX
        DecodeSbcImmediateMode(decodedInstruction);
    else if (opcode == 0x9E) // SBC A, (HL)
        DecodeSbcRegisterIndirectMode(decodedInstruction);
    else if ((opcode >> 3) == 0x13) // SBC A, r'
        DecodeSbcRegisterMode(opcode, decodedInstruction);
}

void InstructionSbc::Execute(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    auto operand1  = registerBank->Read(decodedInstruction.DestinationRegister); // Always A
    auto operand2 = AcquireSourceOperandValue(registerBank, decodedInstruction);
    auto borrow  = registerBank->ReadFlag(Flag::CY);

    registerBank->Write(Register::F, 0x00);
    auto result = CalculateBinarySubtractionAndSetFlags(operand1, operand2, borrow, registerBank);
    registerBank->Write(decodedInstruction.DestinationRegister, static_cast<uint8_t>(result));
}

inline void InstructionSbc::DecodeSbcRegisterMode(uint8_t opcode, DecodedInstruction& decodedInstruction)
{
    auto source = RegisterBankInterface::FromInstructionSource(opcode & 0x07);
    decodedInstruction =
    {
        .Opcode = OpcodeType::sbc,
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

inline void InstructionSbc::DecodeSbcImmediateMode(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::sbc,
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

inline void InstructionSbc::DecodeSbcRegisterIndirectMode(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::sbc,
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