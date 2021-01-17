#pragma once

#include <memory>
#include <optional>

#include <iostream>

#include "../interfaces/RegisterBankInterface.h"
#include "../interfaces/InstructionInterface.h"

namespace gbx
{

class InstructionSubBase
{
public:
    uint8_t Acquire8bitSourceOperandValue(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    uint16_t Acquire16bitSourceOperandValue(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    uint8_t CalculateBinarySubtractionAndSetFlags(uint8_t, uint8_t, std::optional<uint8_t>, std::shared_ptr<interfaces::RegisterBankInterface>);
};

}