#include "LD.h"

using namespace std;

namespace gbx
{
void LD::Decode(uint8_t opcode)
{
    auto prefix = (opcode >> 6);
    auto secondPrefix = (opcode >> 3) & 0x07;
    auto suffix = (opcode & 0x07);
    auto fullSuffix = (opcode & 0x0F);
    
    if (prefix == 0x00 && suffix == 0x06)
        DecodeImmediateOperand(opcode);
    else if (prefix == 0x00 && fullSuffix == 0x0A)
        DecodeRegisterIndirectOperandBCDE(opcode);
    else if (prefix == 0x00 && fullSuffix == 0x02)
        DecodeRegisterIndirectOperandDestinationBCDE(opcode);
    else if (prefix == 0x01 && suffix == 0x06)
        DecodeRegisterIndirectOperandHL(opcode);
    else if (prefix == 0x01 && secondPrefix == 0x06)
        DecodeRegisterIndirectOperandDestinationHL(opcode);
    else if (prefix == 0x01)
        DecodeRegisterRegisterOperand(opcode);
    else 
        throw InstructionException("invalid opcode variant of instruction 'ld'");
}

void LD::Execute(std::shared_ptr<RegisterBank> registerBank, __attribute__((unused)) std::shared_ptr<Channel<MemoryMessage>> memoryChannel)
{
    if (InstructionData == nullopt)
        throw InstructionException("attempted to execute a not-decoded instruction");

    auto addressingMode = InstructionData.value().AddressingMode;
    if (addressingMode == AddressingMode::Immediate || addressingMode == AddressingMode::RegisterIndirectSource)
        ExecuteImmediateOrRegisterIndirectSourceMode(registerBank);
    else if (InstructionData.value().AddressingMode == AddressingMode::Register)
        ExecuteRegisterMode(registerBank);
    else if (addressingMode == AddressingMode::RegisterIndirectDestination)
        ExecuteRegisterIndirectDestinationMode(registerBank);
}

inline void LD::ExecuteImmediateOrRegisterIndirectSourceMode(std::shared_ptr<RegisterBank> registerBank)
{
    registerBank->Write(InstructionData.value().DestinationRegister, InstructionData.value().MemoryOperand1);
}

inline void LD::ExecuteRegisterMode(std::shared_ptr<RegisterBank> registerBank)
{
    auto currentSourceValue = registerBank->Read(InstructionData.value().SourceRegister);
    registerBank->Write(InstructionData.value().DestinationRegister, currentSourceValue);
}

inline void LD::ExecuteRegisterIndirectDestinationMode(std::shared_ptr<RegisterBank> registerBank)
{
    auto currentSourceValue = registerBank->Read(InstructionData.value().SourceRegister);
    InstructionData.value().MemoryResult1 = currentSourceValue;
}

inline void LD::DecodeImmediateOperand(uint8_t opcode)
{
    auto destination = RegisterBank::FromInstructionDestination((opcode >> 3) & 0x07);
    InstructionData = make_optional<DecodedInstruction>({OpcodeType::ld, 
                                                            AddressingMode::Immediate, 
                                                            0x00, 
                                                            Register::NoRegiser, 
                                                            destination,
                                                            0x00});
}

inline void LD::DecodeRegisterIndirectOperandBCDE(uint8_t opcode)
{
    auto source = ((opcode >> 3) & 0x07) == 0x01? Register::BC : Register::DE;
    InstructionData = make_optional<DecodedInstruction>({OpcodeType::ld, 
                                                            AddressingMode::RegisterIndirectSource, 
                                                            0x00, 
                                                            source, 
                                                            Register::A,
                                                            0x00});
}

inline void LD::DecodeRegisterIndirectOperandHL(uint8_t opcode)
{
    auto destination = RegisterBank::FromInstructionDestination((opcode >> 3) & 0x07);
    InstructionData = make_optional<DecodedInstruction>({OpcodeType::ld, 
                                                            AddressingMode::RegisterIndirectSource, 
                                                            0x00, 
                                                            Register::HL, 
                                                            destination,
                                                            0x00});
}

inline void LD::DecodeRegisterIndirectOperandDestinationHL(uint8_t opcode)
{
    auto source = RegisterBank::FromInstructionSource(opcode & 0x07);
    InstructionData = make_optional<DecodedInstruction>({OpcodeType::ld, 
                                                            AddressingMode::RegisterIndirectDestination, 
                                                            0x00, 
                                                            source, 
                                                            Register::HL,
                                                            0x00});
}

inline void LD::DecodeRegisterRegisterOperand(uint8_t opcode)
{
    auto destination = RegisterBank::FromInstructionDestination((opcode >> 3) & 0x07);
    auto source = RegisterBank::FromInstructionSource((opcode) & 0x07); 
    InstructionData = make_optional<DecodedInstruction>({OpcodeType::ld, 
                                                            AddressingMode::Register, 
                                                            0x00, 
                                                            source, 
                                                            destination,
                                                            0x00}); 
}

void LD::DecodeRegisterIndirectOperandDestinationBCDE(uint8_t opcode)
{
    auto destination = ((opcode >> 4) & 0x0F) == 0x01? Register::DE : Register::BC;
    InstructionData = make_optional<DecodedInstruction>({OpcodeType::ld, 
                                                            AddressingMode::RegisterIndirectDestination, 
                                                            0x00, 
                                                            Register::A, 
                                                            destination,
                                                            0x00});
}

}