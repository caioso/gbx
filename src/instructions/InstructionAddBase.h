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
    uint8_t AcquireSourceOperandValue(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    uint8_t CalculateBinaryAdditionAndSetFlags(uint8_t, uint8_t, std::optional<uint8_t>, std::shared_ptr<interfaces::RegisterBankInterface>);
};

}