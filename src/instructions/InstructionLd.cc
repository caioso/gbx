#include "InstructionLd.h"

using namespace std;
using namespace gbx::interfaces;

namespace gbx
{

void InstructionLd::Decode(uint8_t opcode, std::optional<uint8_t> preOpcode, DecodedInstruction& decodedInstruction)
{
    if (preOpcode.has_value())
    {
        if ((opcode >> 0x06) == 0x01 && (opcode & 0x07) == 0x06) // LD r, (IX|IY)
            DecodeRegisterIndexedMove(opcode, preOpcode.value(), decodedInstruction);
        else if ((opcode >> 0x03) == 0x0E) // LD (IX|IY), r
            DecodeIndexedRegisterMove(opcode, preOpcode.value(), decodedInstruction);
    }
    else
    {
        if (opcode == 0xF2) // LD A, (C)
            DecodeAccumulatorRegisterImplicitMove(decodedInstruction);
        else if (opcode == 0xE2) // LD (C), A
            DecodeRegisterImplicitAccumulatorMove(decodedInstruction);
        else if (opcode == 0xF0) // LD A, (n)
            DecodeAccumulatorImmediateImplicitMove(decodedInstruction);
        else if (opcode == 0xE0) // LD (n), A
            DecodeImmediateImplicitAccumulatorMove(decodedInstruction);
        else if (opcode == 0xFA) // LD A, (nn)
            DecodeAccumulatorExtendedImmediateMove(decodedInstruction);
        else if (opcode == 0xEA) // LD (nn), A
            DecodeExtendedImmediateAccumulatorMove(decodedInstruction);
        else if (opcode == 0x08) // LD (nn), SP
            DecodeExtendedImmediatePairMove(decodedInstruction);
        else if (opcode == 0x2A) // LD A, (HL+)
            DecodeAccumulatorPointerIncrementMove(decodedInstruction);
        else if (opcode == 0x3A) // LD A, (HL-)
            DecodeAccumulatorPointerDecrementMove(decodedInstruction);
        else if (opcode == 0x12 || opcode == 0x02) // LD (BC|DE), A
            DecodeSecondaryPointerAccumulatorMove(opcode, decodedInstruction);
        else if (opcode == 0x22) // LD (HL+), A
            DecodePointerIncrementAccumulatorMove(decodedInstruction);
        else if (opcode == 0x32) // LD (HL-), A
            DecodePointerDecrementAccumulatorMove(decodedInstruction);
        else if (opcode == 0x36) // LD (HL), #XX
            DecodePointerImmediateMove(decodedInstruction);
        else if (opcode == 0x0A || opcode == 0x1A) // LD A, (BC|DE)
            DecodeAccumulatorSecondaryPointerMove(opcode, decodedInstruction);
        else if (opcode == 0xF9) // LD SP, HL
            DecodeStackPointerPointerMove(decodedInstruction);
        else if ((opcode >> 0x06) == 0x00 && (opcode & 0x07) == MemoryOperand) // LD r, #XX
            DecodeRegisterImmediateMove(opcode, decodedInstruction);
        else if ((opcode & 0x40) && (opcode & 0x07) == MemoryOperand) // LD r, (HL)
            DecodeRegisterPointerMove(opcode, decodedInstruction);
        else if ((opcode >> 0x06) == 0x00 && (opcode & 0x07) == 0x01) // LD p, #XXXX
            DecodeRegisterPairImmediateMove(opcode, decodedInstruction);
        else if ((opcode >> 0x03) == 0x0E) // LD (HL), r
            DecodePointerRegisterMove(opcode, decodedInstruction);
        else if ((opcode & 0x40) && (opcode & 0x07) != MemoryOperand) // LD r, r'
            DecodeRegisterRegisterMove(opcode, decodedInstruction);
    }
}

void InstructionLd::Execute(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
        if (auto currentAddressingMode = decodedInstruction.AddressingMode;
            InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::Immediate, 
            AddressingMode::RegisterIndirectSource, AddressingMode::RegisterIndirectSourceDecrement,
            AddressingMode::RegisterIndirectSourceIncrement, AddressingMode::RegisterImplicitSource))
            ExecuteRegisterMemoryOperand1Transfer(registerBank, decodedInstruction);
        else if (InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::ImmediatePair))
            ExecuteRegisterMemoryOperand1And2Transfer(registerBank, decodedInstruction);
        else if (InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::RegisterIndexedSource,
                 AddressingMode::ImmediateImplicitSource))
            ExecuteRegisterMemoryOperand2Transfer(registerBank, decodedInstruction);
        else if (InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::Register))
            ExecuteRegisterRegisterTransfer(registerBank, decodedInstruction);
        else if (InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::RegisterIndirectDestination, 
                 AddressingMode::RegisterIndexedDestination, AddressingMode::ExtendedDestination, 
                 AddressingMode::RegisterIndirectDestinationDecrement, AddressingMode::RegisterIndirectDestinationIncrement,
                 AddressingMode::RegisterImplicitDestination, AddressingMode::ImmediateImplicitDestination))
            ExecuteRegisterMemoryTransfer(registerBank, decodedInstruction);
        else if (InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::ExtendedSource))
            ExecuteRegisterMemoryOperand3Transfer(registerBank, decodedInstruction);
        else if (InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::ImmediateRegisterIndirect))
            ExecuteMemoryOperand1MemoryTransfer(decodedInstruction);
        else if (InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::RegisterPair))
            ExecuteRegisterPairRegisterPairTransfer(registerBank, decodedInstruction);
        else if (InstructionUtilities::IsAddressingMode(currentAddressingMode, AddressingMode::ExtendedDestinationPair))
            ExecuteRegisterPairTransferToMemory(registerBank, decodedInstruction);
}

inline void InstructionLd::DecodeRegisterImmediateMove(uint8_t opcode, DecodedInstruction& decodedInstruction)
{
    auto destination = RegisterBankInterface::FromInstructionDestination((opcode >> 3) & 0x07);
    decodedInstruction =
    {
        .Opcode = OpcodeType::ld,
        .AddressingMode = AddressingMode::Immediate,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::NoRegiser,
        .DestinationRegister = destination,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionLd::DecodeRegisterRegisterMove(uint8_t opcode, DecodedInstruction& decodedInstruction)
{
    auto source = RegisterBankInterface::FromInstructionSource(opcode & 0x07);
    auto destination = RegisterBankInterface::FromInstructionDestination((opcode >> 3) & 0x07);
    decodedInstruction =
    {
        .Opcode = OpcodeType::ld,
        .AddressingMode = AddressingMode::Register,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = source,
        .DestinationRegister = destination,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionLd::DecodeRegisterPointerMove(uint8_t opcode, DecodedInstruction& decodedInstruction)
{
    auto destination = RegisterBankInterface::FromInstructionDestination((opcode >> 3) & 0x07);
    decodedInstruction =
    {
        .Opcode = OpcodeType::ld,
        .AddressingMode = AddressingMode::RegisterIndirectSource,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::HL,
        .DestinationRegister = destination,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionLd::DecodePointerRegisterMove(uint8_t opcode, DecodedInstruction& decodedInstruction)
{
    auto source = RegisterBankInterface::FromInstructionSource(opcode & 0x07);

    decodedInstruction =
    {
        .Opcode = OpcodeType::ld,
        .AddressingMode = AddressingMode::RegisterIndirectDestination,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = source,
        .DestinationRegister = Register::HL,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionLd::DecodePointerImmediateMove(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::ld,
        .AddressingMode = AddressingMode::ImmediateRegisterIndirect,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::NoRegiser,
        .DestinationRegister = Register::HL,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionLd::DecodeAccumulatorSecondaryPointerMove(uint8_t opcode, DecodedInstruction& decodedInstruction)
{
    auto source = ((opcode >> 3) & 0x07) == 0x01? Register::BC : Register::DE;
    decodedInstruction =
    {
        .Opcode = OpcodeType::ld,
        .AddressingMode = AddressingMode::RegisterIndirectSource,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = source,
        .DestinationRegister = Register::A,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionLd::DecodeAccumulatorRegisterImplicitMove(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::ld,
        .AddressingMode = AddressingMode::RegisterImplicitSource,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::C,
        .DestinationRegister = Register::A,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionLd::DecodeRegisterImplicitAccumulatorMove(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::ld,
        .AddressingMode = AddressingMode::RegisterImplicitDestination,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::A,
        .DestinationRegister = Register::C,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionLd::DecodeAccumulatorImmediateImplicitMove(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::ld,
        .AddressingMode = AddressingMode::ImmediateImplicitSource,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::NoRegiser,
        .DestinationRegister = Register::A,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionLd::DecodeImmediateImplicitAccumulatorMove(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::ld,
        .AddressingMode = AddressingMode::ImmediateImplicitDestination,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::A,
        .DestinationRegister = Register::NoRegiser,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionLd::DecodeAccumulatorExtendedImmediateMove(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::ld,
        .AddressingMode = AddressingMode::ExtendedSource,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::NoRegiser,
        .DestinationRegister = Register::A,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionLd::DecodeExtendedImmediateAccumulatorMove(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::ld,
        .AddressingMode = AddressingMode::ExtendedDestination,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::A,
        .DestinationRegister = Register::NoRegiser,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionLd::DecodeExtendedImmediatePairMove(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::ld,
        .AddressingMode = AddressingMode::ExtendedDestinationPair,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::SP,
        .DestinationRegister = Register::NoRegiser,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionLd::DecodeAccumulatorPointerIncrementMove(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::ld,
        .AddressingMode = AddressingMode::RegisterIndirectSourceIncrement,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::HL,
        .DestinationRegister = Register::A,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionLd::DecodeAccumulatorPointerDecrementMove(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::ld,
        .AddressingMode = AddressingMode::RegisterIndirectSourceDecrement,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::HL,
        .DestinationRegister = Register::A,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionLd::DecodeSecondaryPointerAccumulatorMove(uint8_t opcode, DecodedInstruction& decodedInstruction)
{
    auto destination = ((opcode >> 4) & 0x0F) == 0x01? Register::DE : Register::BC;
    decodedInstruction =
    {
        .Opcode = OpcodeType::ld,
        .AddressingMode = AddressingMode::RegisterIndirectDestination,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::A,
        .DestinationRegister = destination,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionLd::DecodePointerIncrementAccumulatorMove(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::ld,
        .AddressingMode = AddressingMode::RegisterIndirectDestinationIncrement,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::A,
        .DestinationRegister = Register::HL,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionLd::DecodePointerDecrementAccumulatorMove(DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::ld,
        .AddressingMode = AddressingMode::RegisterIndirectDestinationDecrement,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::A,
        .DestinationRegister = Register::HL,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionLd::DecodeRegisterPairImmediateMove(uint8_t opcode, interfaces::DecodedInstruction& decodedInstruction)
{
    auto destination = RegisterBankInterface::FromInstructionToPair((opcode >> 4) & 0x03);
    decodedInstruction =
    {
        .Opcode = OpcodeType::ld,
        .AddressingMode = AddressingMode::ImmediatePair,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::NoRegiser,
        .DestinationRegister = destination,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionLd::DecodeStackPointerPointerMove(DecodedInstruction& decodedInstruction)
{              
    decodedInstruction =
    {
        .Opcode = OpcodeType::ld,
        .AddressingMode = AddressingMode::RegisterPair,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::HL,
        .DestinationRegister = Register::SP,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionLd::DecodeRegisterIndexedMove(uint8_t opcode, uint8_t preOpcode, interfaces::DecodedInstruction& decodedInstruction)
{
    auto destination = RegisterBankInterface::FromInstructionDestination((opcode >> 3) & 0x07);
    auto source = preOpcode == 0xDD ? Register::IX : Register::IY;

    decodedInstruction =
    {
        .Opcode = OpcodeType::ld,
        .AddressingMode = AddressingMode::RegisterIndexedSource,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = source,
        .DestinationRegister = destination,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionLd::DecodeIndexedRegisterMove(uint8_t opcode, uint8_t preOpcode, interfaces::DecodedInstruction& decodedInstruction)
{
    auto source = RegisterBankInterface::FromInstructionSource(opcode & 0x07);
    auto destination = preOpcode == 0xDD ? Register::IX : Register::IY;

    decodedInstruction =
    {
        .Opcode = OpcodeType::ld,
        .AddressingMode = AddressingMode::RegisterIndexedDestination,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = source,
        .DestinationRegister = destination,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00
    };
}

inline void InstructionLd::ExecuteRegisterMemoryOperand1Transfer(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    registerBank->Write(decodedInstruction.DestinationRegister, decodedInstruction.MemoryOperand1);
}

inline void InstructionLd::ExecuteRegisterMemoryOperand1And2Transfer(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    registerBank->WritePair(decodedInstruction.DestinationRegister, decodedInstruction.MemoryOperand1 | decodedInstruction.MemoryOperand2 << 8);
}

inline void InstructionLd::ExecuteRegisterMemoryOperand3Transfer(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    registerBank->Write(decodedInstruction.DestinationRegister, decodedInstruction.MemoryOperand3);
}

inline void InstructionLd::ExecuteRegisterMemoryOperand2Transfer(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    registerBank->Write(decodedInstruction.DestinationRegister, decodedInstruction.MemoryOperand2);
}

inline void InstructionLd::ExecuteRegisterRegisterTransfer(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    auto source = registerBank->Read(decodedInstruction.SourceRegister);
    registerBank->Write(decodedInstruction.DestinationRegister, source);
}

inline void InstructionLd::ExecuteRegisterMemoryTransfer(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    auto currentSourceValue = registerBank->Read(decodedInstruction.SourceRegister);
    decodedInstruction.MemoryResult1 = currentSourceValue;
}

inline void InstructionLd::ExecuteMemoryOperand1MemoryTransfer(DecodedInstruction& decodedInstruction)
{
    decodedInstruction.MemoryResult1 = decodedInstruction.MemoryOperand1;
}

inline void InstructionLd::ExecuteRegisterPairTransferToMemory(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    auto value = registerBank->ReadPair(decodedInstruction.SourceRegister);
    decodedInstruction.MemoryResult1 = value & 0xFF;
    decodedInstruction.MemoryResult2 = (value >> 0x08) & 0xFF;
}

inline void InstructionLd::ExecuteRegisterPairRegisterPairTransfer(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    auto source = registerBank->ReadPair(decodedInstruction.SourceRegister);
    registerBank->WritePair(decodedInstruction.DestinationRegister, source);
}
}