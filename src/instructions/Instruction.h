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
    Implicit,
    RegisterIndexedSource
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
    virtual void Decode(uint8_t opcode, std::optional<uint8_t> preOpcode) = 0;
    virtual void Execute(std::shared_ptr<RegisterBank> registerBank, std::shared_ptr<Channel<MemoryMessage>> memoryChannel) = 0;

    std::optional<DecodedInstruction> InstructionData;
};

}