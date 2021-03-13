#pragma once

#include <cstdint>
#include <optional>
#include <memory>

#include "ArithmeticLogicUnitInterface.h"
#include "RegisterBankInterface.h"

namespace gbxcore::interfaces
{

class BaseInstructionInterface
{
public:
    virtual ~BaseInstructionInterface() = default;
    virtual void Decode(uint8_t, std::optional<uint8_t>, interfaces::DecodedInstruction&) = 0;
};

class InstructionInterface : public BaseInstructionInterface
{
public:
    virtual ~InstructionInterface() = default;
    virtual void Execute(std::shared_ptr<RegisterBankInterface>, DecodedInstruction&) = 0;
};

class ConditionalInstructionInterface : public BaseInstructionInterface
{
public:
    virtual ~ConditionalInstructionInterface() = default;
    virtual bool ConditionallyExecute(std::shared_ptr<RegisterBankInterface>, DecodedInstruction&) = 0;
};

}