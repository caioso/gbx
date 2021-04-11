#include "InstructionAddBase.h"

using namespace std;
using namespace gbxcore::interfaces;

namespace gbxcore::instructions
{

uint8_t InstructionAddBase::Calculate8BitBinaryAdditionAndSetFlags(uint8_t operand1, uint8_t operand2, optional<uint8_t> carry, interfaces::RegisterBankInterface* registerBank)
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

uint16_t InstructionAddBase::Calculate16BitBinaryAdditionAndSetFlags(uint16_t operand1, uint16_t operand2, optional<uint8_t> carry, interfaces::RegisterBankInterface* registerBank, FlagMode mode)
{
    auto result = static_cast<uint16_t>(0x00);
    auto carryIn = carry.value_or(0x00);

    for (auto i = 0; i < 16; i++)
    {
        auto operand1Bit = (operand1 >> i) & 0x01;
        auto operand2Bit = (operand2 >> i) & 0x01;
        auto resultBit = (operand1Bit ^ operand2Bit) ^ carryIn;
        carryIn = (operand1Bit & operand2Bit) | (carryIn & (operand1Bit ^ operand2Bit));

        if (mode == FlagMode::Flag16BitMode)
        {
            if (i == 11 && carryIn)
                registerBank->SetFlag(Flag::H);
            else if (i == 15 && carryIn)
                registerBank->SetFlag(Flag::CY);
        }
        else
        {
            if (i == 3 && carryIn)
                registerBank->SetFlag(Flag::H);
            else if (i == 7 && carryIn)
                registerBank->SetFlag(Flag::CY);
        }

        result |= (resultBit << i);
    }

    if (result == 0)
        registerBank->SetFlag(Flag::Z);
    registerBank->ClearFlag(Flag::N);

    return result;
}

FlagMode InstructionAddBase::DecideFlagMode(interfaces::DecodedInstruction& decodedInstruction)
{
    return decodedInstruction.AddressingMode == AddressingMode::RegisterPair ? FlagMode::Flag16BitMode : FlagMode::Flag8BitMode;
}

uint8_t InstructionAddBase::Acquire8BitSourceOperandValue(interfaces::RegisterBankInterface* registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.AddressingMode == AddressingMode::Register)
        return registerBank->Read(decodedInstruction.SourceRegister);
    else
        return decodedInstruction.MemoryOperand1;
}

uint16_t InstructionAddBase::Acquire16BitSourceOperandValue(interfaces::RegisterBankInterface* registerBank, DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.AddressingMode == AddressingMode::RegisterPair)
        return registerBank->ReadPair(decodedInstruction.SourceRegister);
    else 
        return static_cast<uint16_t>((decodedInstruction.MemoryOperand1 >> 7 != 0? 0xFF00 : 0x0000) | decodedInstruction.MemoryOperand1);
}

}