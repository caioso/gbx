#pragma once

#include <cstdint>
#include <optional>
#include <memory>

#include "ArithmeticLogicUnitInterface.h"
#include "RegisterBankInterface.h"

namespace gbx::interfaces
{

class InstructionInterface
{
public:
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) = 0;
    virtual void Execute(std::shared_ptr<RegisterBankInterface>, DecodedInstruction&) = 0;
};

}