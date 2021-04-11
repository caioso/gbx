#pragma once

#include <memory>
#include <optional>

#include <iostream>

#include "RegisterBankInterface.h"
#include "InstructionInterface.h"

namespace gbxcore::instructions
{

enum FlagMode
{
    Flag16BitMode,
    Flag8BitMode
};

class InstructionAddBase
{
public:
    uint8_t Acquire8BitSourceOperandValue(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&);
    uint8_t Calculate8BitBinaryAdditionAndSetFlags(uint8_t, uint8_t, std::optional<uint8_t>, interfaces::RegisterBankInterface*);
    uint16_t Acquire16BitSourceOperandValue(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&);
    uint16_t Calculate16BitBinaryAdditionAndSetFlags(uint16_t, uint16_t, std::optional<uint8_t>, interfaces::RegisterBankInterface*, FlagMode);
    FlagMode DecideFlagMode(interfaces::DecodedInstruction&);
};

}