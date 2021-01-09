#include "InstructionSubBase.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx
{

uint8_t InstructionSubBase::AcquireSourceOperandValue(shared_ptr<interfaces::RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.AddressingMode == AddressingMode::Register)
        return registerBank->Read(decodedInstruction.SourceRegister);
    else
        return decodedInstruction.MemoryOperand1;
}

uint8_t InstructionSubBase::CalculateBinarySubtractionAndSetFlags(uint8_t operand1, uint8_t operand2, shared_ptr<RegisterBankInterface> registerBank)
{
    auto result = static_cast<uint8_t>(0x00);
    auto borrowIn = static_cast<uint8_t>(0x00);

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

    return result;
}

}