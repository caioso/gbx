#pragma once

#include <cstdint>
#include <optional>
#include <memory>

#include "../MemoryController.h"
#include "../RegisterBank.h"

namespace gbx
{

enum class OpcodeType
{
    unknown,
    ld
};

enum class AddressingMode
{
    Register,
    Immediate,
    RegisterIndirectSource,
    RegisterIndirectDestination,
    Implicit,
    RegisterIndexedSource,
    RegisterIndexedDestination,
};

struct DecodedInstruction
{
    OpcodeType Opcode;
    AddressingMode AddressingMode;
    uint8_t MemoryOperand1;
    uint8_t MemoryOperand2;
    Register SourceRegister;
    Register DestinationRegister;
    uint8_t MemoryResult1;
};

class Instruction
{
public:
    virtual ~Instruction() = default;    
    virtual void Decode(uint8_t, std::optional<uint8_t>) = 0;
    virtual void Execute(std::shared_ptr<RegisterBank>) = 0;

    std::optional<DecodedInstruction> InstructionData;
};

}