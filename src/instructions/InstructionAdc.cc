#include "InstructionAdc.h"

using namespace gbxcore::interfaces;
using namespace std; 

namespace gbxcore::instructions
{

void InstructionAdc::Decode(uint8_t opcode, __attribute__((unused)) optional<uint8_t> preOpcode, DecodedInstruction& decodedInstruction)
{
    if (opcode == 0xCE) // ADC A, #XX
        DecodeAddImmediateMode(decodedInstruction);
    else if (opcode == 0x8E) // ADC A, (HL)
        DecodeAddRegisterIndirectMode(decodedInstruction);
    else if ((opcode >> 0x03) == 0x11) // ADC A, r'
        DecodeAddRegisterMode(opcode, decodedInstruction);
}

void InstructionAdc::Execute(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    auto operand1 = Acquire8BitSourceOperandValue(registerBank, decodedInstruction);
    auto operand2  = registerBank->Read(decodedInstruction.DestinationRegister); // Always A
    auto carry  = registerBank->ReadFlag(Flag::CY);
    
    registerBank->Write(Register::F, 0x00);
    auto result = Calculate8BitBinaryAdditionAndSetFlags(operand1, operand2, carry, registerBank);
    registerBank->Write(decodedInstruction.DestinationRegister, static_cast<uint8_t>(result));
}

void InstructionAdc::DecodeAddRegisterMode(uint8_t opcode, interfaces::DecodedInstruction& decodedInstruction)
{
    auto source = RegisterBankInterface::FromInstructionSource(opcode & 0x07);
    decodedInstruction =
    {
        .Opcode = OpcodeType::adc,
        .AddressingMode = AddressingMode::Register,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = source,
        .DestinationRegister = Register::A,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0x00
    };
}

void InstructionAdc::DecodeAddImmediateMode(interfaces::DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::adc,
        .AddressingMode = AddressingMode::Immediate,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::NoRegister,
        .DestinationRegister = Register::A,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0x00
    };
}

void InstructionAdc::DecodeAddRegisterIndirectMode(interfaces::DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::adc,
        .AddressingMode = AddressingMode::RegisterIndirectSource,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::HL,
        .DestinationRegister = Register::A,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0x00
    };
}

}