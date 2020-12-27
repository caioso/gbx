#include "LD.h"

using namespace std;

namespace gbx
{
void LD::Decode(uint8_t opcode)
{
    auto prefix = (opcode >> 6);
    auto suffix = (opcode & 0x07);
    
    if (prefix == 0x00 && suffix == 0x06)
    {
        auto destination = RegisterBank::FromInstructionDestination((opcode >> 3) & 0x07);
        InstructionData = make_optional<DecodedInstruction>({OpcodeType::ld, 
                                                             AddressingMode::Immediate, 
                                                             0x00, 
                                                             Register::NoRegiser, 
                                                             destination});
    }
    else if (prefix == 0x01)
    {
        auto destination = RegisterBank::FromInstructionDestination((opcode >> 3) & 0x07);
        auto source = RegisterBank::FromInstructionSource((opcode) & 0x07); 
        InstructionData = make_optional<DecodedInstruction>({OpcodeType::ld, 
                                                             AddressingMode::Register, 
                                                             0x00, 
                                                             source, 
                                                             destination});
    }
    else 
    {
        throw InstructionException("invalid opcode variant of instruction 'ld'");
    }
}

void LD::Execute(std::shared_ptr<RegisterBank> registerBank, __attribute__((unused)) std::shared_ptr<Channel<MemoryMessage>> memoryChannel)
{
    if (InstructionData == nullopt)
        throw InstructionException("attempted to execute a not-decoded instruction");

    if (InstructionData.value().AddressingMode == AddressingMode::Immediate)
    {
        registerBank->Write(InstructionData.value().DestinationRegister, InstructionData.value().MemoryOperand1);
    }
    else if (InstructionData.value().AddressingMode == AddressingMode::Register)
    {
        auto currentSourceValue = registerBank->Read(InstructionData.value().SourceRegister);
        registerBank->Write(InstructionData.value().DestinationRegister, currentSourceValue);
    }
}

}