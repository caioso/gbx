#pragma once

#include <memory>
#include <optional>

#include "RegisterBankInterface.h"
#include "InstructionInterface.h"

namespace gbxcore::instructions
{

class InstructionSubBase
{
public:
    uint8_t Acquire8bitSourceOperandValue(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&);
    uint16_t Acquire16bitSourceOperandValue(interfaces::RegisterBankInterface*, interfaces::DecodedInstruction&);
    uint8_t CalculateBinarySubtractionAndSetFlags(uint8_t, uint8_t, std::optional<uint8_t>, interfaces::RegisterBankInterface*);
};

}