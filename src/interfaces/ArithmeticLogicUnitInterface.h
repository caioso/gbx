#pragma once

#include <cstdint>
#include <optional>
#include <memory>

#include "../AddressingMode.h"
#include "../MemoryController.h"
#include "../RegisterBank.h"
#include "../instructions/Opcodes.h"

namespace gbx::interfaces
{

struct DecodedInstruction
{
    OpcodeType Opcode;
    AddressingMode AddressingMode;
    uint8_t MemoryOperand1;
    uint8_t MemoryOperand2;
    uint8_t MemoryOperand3;
    Register SourceRegister;
    Register DestinationRegister;
    uint8_t MemoryResult1;
};
class InstructionConstants
{
public:
    const static uint16_t PreOpcode_DD = 0xDD;
    const static uint16_t PreOpcode_FD = 0xFD;
};

class ArithmeticLogicUnitInterface
{
public:
    virtual ~ArithmeticLogicUnitInterface() = default;    
    virtual void Decode(uint8_t, std::optional<uint8_t>) = 0;
    virtual void Execute(std::shared_ptr<RegisterBank>) = 0;

    DecodedInstruction InstructionData;
};


}