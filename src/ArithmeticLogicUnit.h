#pragma once

#include <memory>

#include "GBXExceptions.h"
#include "interfaces/InstructionInterface.h"
#include "instructions/InstructionUtilities.h"
#include "instructions/OpcodeDecoder.h"
#include "instructions/OpcodePatternMatcher.h"
#include "interfaces/ArithmeticLogicUnitInterface.h"

namespace gbx
{

class ArithmeticLogicUnit : public interfaces::ArithmeticLogicUnitInterface
{
public:
    ArithmeticLogicUnit() = default;
    virtual ~ArithmeticLogicUnit() = default;
    virtual void Decode(uint8_t, std::optional<uint8_t>) override;
    virtual void Execute(std::shared_ptr<RegisterBank>) override;

private:
    std::shared_ptr<interfaces::InstructionInterface> _currentInstruction;
    OpcodeDecoder _decoder;
};

}