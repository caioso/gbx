#include "InstructionAddBase.h"

using namespace std;
using namespace gbx::interfaces;

namespace gbx
{

uint8_t InstructionAddBase::CalculateBinaryAdditionAndSetFlags(uint8_t operand1, uint8_t operand2, optional<uint8_t> carry, shared_ptr<RegisterBankInterface> registerBank)
{
    auto result = static_cast<uint8_t>(0x00);
    auto carryIn = carry.value_or(0x00);

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

uint8_t InstructionAddBase::AcquireSourceOperandValue(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.AddressingMode == AddressingMode::Register)
        return registerBank->Read(decodedInstruction.SourceRegister);
    else
        return decodedInstruction.MemoryOperand1;
}

}