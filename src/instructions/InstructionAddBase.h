#pragma once

#include <memory>
#include <optional>

#include <iostream>

#include "../interfaces/RegisterBankInterface.h"
#include "../interfaces/InstructionInterface.h"

namespace gbx
{

class InstructionAddBase
{
public:
    uint8_t Acquire8BitSourceOperandValue(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    uint8_t Calculate8BitBinaryAdditionAndSetFlags(uint8_t, uint8_t, std::optional<uint8_t>, std::shared_ptr<interfaces::RegisterBankInterface>);
    uint16_t Acquire16BitSourceOperandValue(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    uint16_t Calculate16BitBinaryAdditionAndSetFlags(uint16_t, uint16_t, std::optional<uint8_t>, std::shared_ptr<interfaces::RegisterBankInterface>);
};

}