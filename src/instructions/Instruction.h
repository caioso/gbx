#pragma once

#include <cstdint>
#include <optional>
#include <memory>

#include "../Channel.h"
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
    Implicit
};

struct DecodedInstruction
{
    OpcodeType Opcode;
    AddressingMode AddressingMode;
    uint8_t MemoryOperand1;
    Register SourceRegister;
    Register DestinationRegister;
};

class Instruction
{
public:
    virtual ~Instruction() = default;    
    virtual void Decode(uint8_t opcode) = 0;
    virtual void Execute(std::shared_ptr<RegisterBank> registerBank, std::shared_ptr<Channel<MemoryMessage>> memoryChannel) = 0;

    std::optional<DecodedInstruction> InstructionData;
};

}