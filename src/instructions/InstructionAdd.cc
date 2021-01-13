#include "InstructionAdd.h"

using namespace std;
using namespace gbx::interfaces;

namespace gbx
{

void InstructionAdd::Decode(uint8_t opcode, __attribute__((unused)) optional<uint8_t> preOpcode, DecodedInstruction& decodeInstruction)
{
    if (opcode == 0x86)  // ADD A, (HL))
        DecodeAddPointerMode(decodeInstruction);
    else if (opcode == 0xC6)  // ADD A, #XX
        DecodeAddImmediateMode(decodeInstruction);
    else if ((opcode >> 0x03) == 0x10)  // ADD A, r
        DecodeAddRegisterMode(opcode, decodeInstruction);
}

void InstructionAdd::Execute(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    auto operand1 = AcquireSourceOperandValue(registerBank, decodedInstruction);
    auto operand2  = registerBank->Read(decodedInstruction.DestinationRegister); // Always A
    
    registerBank->Write(Register::F, 0x00);
    auto result = CalculateBinaryAdditionAndSetFlags(operand1, operand2, nullopt, registerBank);
    registerBank->Write(decodedInstruction.DestinationRegister, static_cast<uint8_t>(result));
}

inline void InstructionAdd::DecodeAddRegisterMode(uint8_t opcode, DecodedInstruction& decodedInstruction)
{
    auto source = RegisterBankInterface::FromInstructionSource(opcode & 0x07);
    decodedInstruction =
    {
        .Opcode = OpcodeType::add,
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

inline void InstructionAdd::DecodeAddImmediateMode(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::add,
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

inline void InstructionAdd::DecodeAddPointerMode(interfaces::DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::add,
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