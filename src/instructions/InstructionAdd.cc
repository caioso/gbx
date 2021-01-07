#include "InstructionAdd.h"

using namespace std;
using namespace gbx::interfaces;

namespace gbx
{

void InstructionAdd::Decode(uint8_t opcode, __attribute__((unused)) std::optional<uint8_t> preOpcode, DecodedInstruction& decodeInstruction)
{
    if ((opcode >> 0x03) == 0x10)  // ADD A, r
        DecodeAddRegisterMode(opcode, decodeInstruction);
    else if (opcode & 0xC6)  // ADD A, #XX
        DecodeAddImmediateMode(decodeInstruction);
}

void InstructionAdd::Execute(shared_ptr<RegisterBank> registerBank, DecodedInstruction& decodedInstruction)
{
    auto operand1 = AcquireSourceOperandValue(registerBank, decodedInstruction);
    auto operand2  = registerBank->Read(decodedInstruction.DestinationRegister); // Always A
    
    registerBank->Write(Register::F, 0x00);
    auto result = CalculateBinaryAdditionAndSetFlags(operand1, operand2, registerBank);
    registerBank->Write(decodedInstruction.DestinationRegister, static_cast<uint8_t>(result));
}

inline uint8_t InstructionAdd::CalculateBinaryAdditionAndSetFlags(uint8_t operand1, uint8_t operand2 , shared_ptr<RegisterBank> registerBank)
{
    auto result = static_cast<uint8_t>(0x00);
    auto carryIn = static_cast<uint8_t>(0x00);

    for (auto i = 0; i < 8; i++)
    {
        auto operand1Bit = (operand1 >> i) & 0x01;
        auto operand2Bit = (operand2 >> i) & 0x01;
        auto resultBit = (operand1Bit ^ operand2Bit) ^ carryIn;
        carryIn = (operand1Bit & operand2Bit) | (carryIn & (operand1Bit ^ operand2Bit));

        if (i == 3 && carryIn)
            registerBank->SetFlag(Flag::H);
        else if (i == 7 && carryIn)
            registerBank->SetFlag(Flag::CY);

        result |= (resultBit << i);
    }

    if (result == 0)
        registerBank->SetFlag(Flag::Z);
    registerBank->ClearFlag(Flag::N);

    return result;
}

inline uint8_t InstructionAdd::AcquireSourceOperandValue(shared_ptr<RegisterBank> registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.AddressingMode == AddressingMode::Register)
        return registerBank->Read(decodedInstruction.SourceRegister);
    else
        return decodedInstruction.MemoryOperand1;
}

inline void InstructionAdd::DecodeAddRegisterMode(uint8_t opcode, DecodedInstruction& decodedInstruction)
{
    auto source = RegisterBank::FromInstructionSource(opcode & 0x07);
    decodedInstruction =
    {
        .Opcode = OpcodeType::add,
        .AddressingMode = AddressingMode::Register,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = source,
        .DestinationRegister = Register::A,
        .MemoryResult1 = 0x00
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
        .MemoryResult1 = 0x00
    };
}
}