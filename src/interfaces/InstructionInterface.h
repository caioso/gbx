#pragma once

#include <cstdint>
#include <optional>
#include <memory>

#include "../RegisterBank.h"
#include "../interfaces/ArithmeticLogicUnitInterface.h"

namespace gbx::interfaces
{

class InstructionInterface
{
public:
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) = 0;
    virtual void Execute(std::shared_ptr<RegisterBank>, DecodedInstruction&) = 0;
};

}