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
    uint8_t AcquireSourceOperandValue(std::shared_ptr<interfaces::RegisterBankInterface>, interfaces::DecodedInstruction&);
    uint8_t CalculateBinarySubtractionAndSetFlags(uint8_t, uint8_t, std::shared_ptr<interfaces::RegisterBankInterface>);
};

}