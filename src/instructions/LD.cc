#include "LD.h"

using namespace std;

namespace gbx
{
void LD::Decode(uint8_t opcode, std::optional<uint8_t> preOpcode)
{
    if (preOpcode.has_value() && (preOpcode.value() == InstructionConstants::PreOpcode_DD || preOpcode.value() == InstructionConstants::PreOpcode_FD))
    {
        
        if (OpcodePatternMatcher::Match(opcode, // 01XX X110
            OpcodePatternMatcher::Pattern(b::_0, b::_1, b::_X, b::_X, b::_X, b::_1, b::_1, b::_0)))
            DecodeRegisterIndexedSource(opcode, preOpcode.value());
        else if (OpcodePatternMatcher::Match(opcode, // 0111 0XXX
                 OpcodePatternMatcher::Pattern(b::_0, b::_1, b::_1, b::_1, b::_0, b::_X, b::_X, b::_X)))
            DecodeRegisterIndexedDestination(opcode, preOpcode.value());
        else 
            throw InstructionException("invalid opcode variant of instruction 'ld'");
    }
    else
    {
        if (OpcodePatternMatcher::Match(opcode, // 0011 0110
            OpcodePatternMatcher::Pattern(b::_0, b::_0, b::_1, b::_1, b::_0, b::_1, b::_1, b::_0)))
            DecodeImmediateRegisterIndirect();
        else if (OpcodePatternMatcher::Match(opcode, // 001X 1010
            OpcodePatternMatcher::Pattern(b::_0, b::_0, b::_1, b::_X, b::_1, b::_0, b::_1, b::_0)))
            DecodeRegisterIndirectSourceIncrementDecrement(opcode);
        else if (OpcodePatternMatcher::Match(opcode, // 001X 0010
            OpcodePatternMatcher::Pattern(b::_0, b::_0, b::_1, b::_X, b::_0, b::_0, b::_1, b::_0)))
            DecodeRegisterIndirectDestinationIncrementDecrement(opcode);
        else if (OpcodePatternMatcher::Match(opcode, // 00XX X110
            OpcodePatternMatcher::Pattern(b::_0, b::_0, b::_X, b::_X, b::_X, b::_1, b::_1, b::_0)))
            DecodeImmediateOperand(opcode);
        else if (OpcodePatternMatcher::Match(opcode, // 000X 1010
                 OpcodePatternMatcher::Pattern(b::_0, b::_0, b::_0, b::_X, b::_1, b::_0, b::_1, b::_0)))
            DecodeRegisterIndirectOperandSourceBCDE(opcode);
        else if (OpcodePatternMatcher::Match(opcode, // 000X 0010
                 OpcodePatternMatcher::Pattern(b::_0, b::_0, b::_0, b::_X, b::_0, b::_0, b::_1, b::_0)))
            DecodeRegisterIndirectOperandDestinationBCDE(opcode);
        else if (OpcodePatternMatcher::Match(opcode, // 0111 0XXX
                 OpcodePatternMatcher::Pattern(b::_0, b::_1, b::_1, b::_1, b::_0, b::_X, b::_X, b::_X)))
            DecodeRegisterIndirectOperandDestinationHL(opcode);
        else if (OpcodePatternMatcher::Match(opcode, // 111X 1010
                 OpcodePatternMatcher::Pattern(b::_1, b::_1, b::_1, b::_X, b::_1, b::_0, b::_1, b::_0)))
            DecodeExtendedOperand(opcode);
        else if (OpcodePatternMatcher::Match(opcode, // 01XX X110
                 OpcodePatternMatcher::Pattern(b::_0, b::_1, b::_X, b::_X, b::_X, b::_1, b::_1, b::_0)))
            DecodeRegisterIndirectOperandSourceHL(opcode);
        else if (OpcodePatternMatcher::Match(opcode, // 01XX XXXX
                 OpcodePatternMatcher::Pattern(b::_0, b::_1, b::_X, b::_X, b::_X, b::_X, b::_X, b::_X)))
            DecodeRegisterRegisterOperand(opcode);
        else 
            throw InstructionException("invalid opcode variant of instruction 'ld'");
    }
}

void LD::Execute(std::shared_ptr<RegisterBank> registerBank)
{
    if (InstructionData == nullopt)
        throw InstructionException("attempted to execute a not-decoded instruction");

    if (auto currentAddressingMode = InstructionData.value().AddressingMode;
        InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::Immediate, 
                                                                      AddressingMode::RegisterIndirectSource,
                                                                      AddressingMode::RegisterIndirectSourceDecrement,
                                                                      AddressingMode::RegisterIndirectSourceIncrement))
        ExecuteMemoryBasedSource(registerBank);
    else if (InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::RegisterIndexedSource))
        ExecuteIndexedSource(registerBank);
    else if (InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::Register))
        ExecuteRegisterSourceOrDestination(registerBank);
    else if (InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::RegisterIndirectDestination, 
                                                                           AddressingMode::RegisterIndexedDestination, 
                                                                           AddressingMode::ExtendedDestination, 
                                                                           AddressingMode::RegisterIndirectDestinationDecrement,
                                                                           AddressingMode::RegisterIndirectDestinationIncrement))
        ExecuteMemoryBasedDestination(registerBank);
    else if (InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::ExtendedSource))
        ExecuteExtendedSource(registerBank);
    else if (InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::ImmediateRegisterIndirect))
        ExecuteImediateRegisterIndirect();
}

inline void LD::ExecuteMemoryBasedSource(std::shared_ptr<RegisterBank> registerBank)
{
    registerBank->Write(InstructionData.value().DestinationRegister, InstructionData.value().MemoryOperand1);
}

inline void LD::ExecuteIndexedSource(std::shared_ptr<RegisterBank> registerBank)
{
    registerBank->Write(InstructionData.value().DestinationRegister, InstructionData.value().MemoryOperand2);
}

inline void LD::ExecuteRegisterSourceOrDestination(std::shared_ptr<RegisterBank> registerBank)
{
    auto currentSourceValue = registerBank->Read(InstructionData.value().SourceRegister);
    registerBank->Write(InstructionData.value().DestinationRegister, currentSourceValue);
}

inline void LD::ExecuteMemoryBasedDestination(std::shared_ptr<RegisterBank> registerBank)
{
    auto currentSourceValue = registerBank->Read(InstructionData.value().SourceRegister);
    InstructionData.value().MemoryResult1 = currentSourceValue;
}

inline void LD::ExecuteExtendedSource(std::shared_ptr<RegisterBank> registerBank)
{
    registerBank->Write(InstructionData.value().DestinationRegister, InstructionData.value().MemoryOperand3);
}

inline void LD::ExecuteImediateRegisterIndirect()
{
    InstructionData.value().MemoryResult1 = InstructionData.value().MemoryOperand1;
}

inline void LD::DecodeImmediateOperand(uint8_t opcode)
{
    auto destination = RegisterBank::FromInstructionDestination((opcode >> 3) & 0x07);
    InstructionData = make_optional<DecodedInstruction>({OpcodeType::ld, 
                                                            AddressingMode::Immediate, 
                                                            0x00,
                                                            0x00, 
                                                            0x00,
                                                            Register::NoRegiser, 
                                                            destination,
                                                            0x00});
}

inline void LD::DecodeRegisterIndirectOperandSourceBCDE(uint8_t opcode)
{
    auto source = ((opcode >> 3) & 0x07) == 0x01? Register::BC : Register::DE;
    InstructionData = make_optional<DecodedInstruction>({OpcodeType::ld, 
                                                            AddressingMode::RegisterIndirectSource, 
                                                            0x00,
                                                            0x00,
                                                            0x00, 
                                                            source, 
                                                            Register::A,
                                                            0x00});
}

inline void LD::DecodeRegisterIndirectOperandSourceHL(uint8_t opcode)
{
    auto destination = RegisterBank::FromInstructionDestination((opcode >> 3) & 0x07);
    InstructionData = make_optional<DecodedInstruction>({OpcodeType::ld, 
                                                            AddressingMode::RegisterIndirectSource, 
                                                            0x00,
                                                            0x00,
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
                                                            0x00,
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
                                                            0x00,
                                                            0x00, 
                                                            source, 
                                                            destination,
                                                            0x00}); 
}

inline void LD::DecodeRegisterIndirectOperandDestinationBCDE(uint8_t opcode)
{
    auto destination = ((opcode >> 4) & 0x0F) == 0x01? Register::DE : Register::BC;
    InstructionData = make_optional<DecodedInstruction>({OpcodeType::ld, 
                                                            AddressingMode::RegisterIndirectDestination, 
                                                            0x00,
                                                            0x00,
                                                            0x00, 
                                                            Register::A, 
                                                            destination,
                                                            0x00});
}

inline void LD::DecodeRegisterIndexedSource(uint8_t opcode, uint8_t preOpcode)
{
    auto destination = RegisterBank::FromInstructionDestination((opcode >> 3) & 0x07);
    auto indexRegister = preOpcode == 0xDD ? Register::IX : Register::IY;
    InstructionData = make_optional<DecodedInstruction>({OpcodeType::ld, 
                                                            AddressingMode::RegisterIndexedSource, 
                                                            0x00,
                                                            0x00,
                                                            0x00, 
                                                            indexRegister, 
                                                            destination,
                                                            0x00});
}

inline void LD::DecodeRegisterIndexedDestination(uint8_t opcode, uint8_t preOpcode)
{
    auto source = RegisterBank::FromInstructionSource(opcode & 0x07);
    auto indexRegister = preOpcode == 0xDD ? Register::IX : Register::IY;
    InstructionData = make_optional<DecodedInstruction>({OpcodeType::ld, 
                                                         AddressingMode::RegisterIndexedDestination, 
                                                         0x00,
                                                         0x00, 
                                                         0x00,
                                                         source, 
                                                         indexRegister,
                                                         0x00});
}

inline void LD::DecodeExtendedOperand(uint8_t opcode)
{
    if (opcode == 0xFA)
    {
        InstructionData = make_optional<DecodedInstruction>({OpcodeType::ld, 
                                                         AddressingMode::ExtendedSource, 
                                                         0x00,
                                                         0x00,
                                                         0x00, 
                                                         Register::NoRegiser, 
                                                         Register::A,
                                                         0x00});
    }
    else
    {
        InstructionData = make_optional<DecodedInstruction>({OpcodeType::ld, 
                                                         AddressingMode::ExtendedDestination, 
                                                         0x00,
                                                         0x00, 
                                                         0x00,
                                                         Register::A,
                                                         Register::NoRegiser, 
                                                         0x00});
    }
}

inline void LD::DecodeImmediateRegisterIndirect()
{
    InstructionData = make_optional<DecodedInstruction>({OpcodeType::ld, 
                                                         AddressingMode::ImmediateRegisterIndirect, 
                                                         0x00,
                                                         0x00, 
                                                         0x00,
                                                         Register::NoRegiser, 
                                                         Register::HL,
                                                         0x00});
}

inline void LD::DecodeRegisterIndirectSourceIncrementDecrement(uint8_t opcode)
{
    if (opcode == 0x2A)
    {
        InstructionData = make_optional<DecodedInstruction>({OpcodeType::ld, 
                                                         AddressingMode::RegisterIndirectSourceIncrement, 
                                                         0x00,
                                                         0x00, 
                                                         0x00,
                                                         Register::HL, 
                                                         Register::A,
                                                         0x00});
    }
    else
    {
        InstructionData = make_optional<DecodedInstruction>({OpcodeType::ld, 
                                                         AddressingMode::RegisterIndirectSourceDecrement, 
                                                         0x00,
                                                         0x00, 
                                                         0x00,
                                                         Register::HL, 
                                                         Register::A,
                                                         0x00});
    }
    
}

inline void LD::DecodeRegisterIndirectDestinationIncrementDecrement(uint8_t opcode)
{
    if (opcode == 0x22)
    {
        InstructionData = make_optional<DecodedInstruction>({OpcodeType::ld, 
                                                         AddressingMode::RegisterIndirectDestinationIncrement, 
                                                         0x00,
                                                         0x00, 
                                                         0x00,
                                                         Register::A, 
                                                         Register::HL,
                                                         0x00});
    }
    else
    {
        InstructionData = make_optional<DecodedInstruction>({OpcodeType::ld, 
                                                         AddressingMode::RegisterIndirectDestinationDecrement, 
                                                         0x00,
                                                         0x00, 
                                                         0x00,
                                                         Register::A, 
                                                         Register::HL,
                                                         0x00});
    }
    
}

}