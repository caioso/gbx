#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <optional>
#include <random>
#include <vector>

#include <iostream>

#include "../src/MemoryController.h"
#include "../src/RegisterBank.h"
#include "../src/ArithmeticLogicUnit.h"
#include "../src/instructions/Opcodes.h"

using namespace std;
using namespace gbx;

uint8_t BinaryRegisterAddressingMode(Register source, Register destination)
{
    return RegisterBank::ToInstructionSource(source) | 
           RegisterBank::ToInstructionDestination(destination) << 3 |
           0x01 << 6;
}

uint8_t BinaryImmediateAddressingMode(Register destination)
{
    return 0x06 | 
           RegisterBank::ToInstructionDestination(destination) << 3 |
           0x00 << 6;
}

uint8_t BinaryRegisterIndirectAsSourceAddressingMode(Register destination)
{
    return 0x06 | 
           RegisterBank::ToInstructionDestination(destination) << 3 |
           0x01 << 6;
}

uint8_t BinaryRegisterIndirectAsDestinationAddressingMode(Register source)
{
    return RegisterBank::ToInstructionSource(source) | 
           0x06 << 3 |
           0x01 << 6;
}

uint8_t BinaryRegisterIndexedSourceAddressingMode(Register destination)
{
    return 0x06 | 
           RegisterBank::ToInstructionDestination(destination) << 3 | 
           0x01 << 6;
}

uint8_t BinaryRegisterIndexedDestinationAddressingMode(Register source)
{
    return RegisterBank::ToInstructionSource(source) | 
           (0x07 << 4);
}

uint8_t BinaryImmediateRegisterPair(Register source)
{
    return (RegisterBank::ToInstructionRegisterPair(source) << 4) | 0x01;
}

TEST(TestALU, ExecuteUndecodedInstruction)
{
    auto registerBank = make_shared<RegisterBank>();
    auto testPassed = false;

    try
    {
        ArithmeticLogicUnit alu;
        alu.Execute(registerBank);
    }
    catch (const InstructionException& e)
    {
        testPassed = true;
    }

    EXPECT_TRUE(testPassed);
}
   

TEST(TestALU, DecodeImmediateAddressingMode)
{
    auto destinationsList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    for (auto destination : destinationsList)
    {
        ArithmeticLogicUnit alu;
        auto rawBinary = BinaryImmediateAddressingMode(destination);
        alu.Decode(rawBinary, nullopt);

        EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
        EXPECT_EQ(AddressingMode::Immediate, alu.InstructionData.AddressingMode);
        EXPECT_EQ(Register::NoRegiser, alu.InstructionData.SourceRegister);
        EXPECT_EQ(destination, alu.InstructionData.DestinationRegister);
    }
}

TEST(TestALU, DecodingRegisterAddressingMode)
{
    auto sourcesList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    auto destinationsList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};

    for (auto source : sourcesList)
    for (auto destination : destinationsList)
    {
        ArithmeticLogicUnit alu;
        auto rawBinary = BinaryRegisterAddressingMode(source, destination);
        alu.Decode(rawBinary, nullopt);

        EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
        EXPECT_EQ(AddressingMode::Register, alu.InstructionData.AddressingMode);
        EXPECT_EQ(source, alu.InstructionData.SourceRegister);
        EXPECT_EQ(destination, alu.InstructionData.DestinationRegister);
    }

    for (auto destination : destinationsList)
    {
        ArithmeticLogicUnit alu;
        constexpr auto opcode = 0x01 << 6;
        constexpr auto forbiddenSourceRegister = (0x06);
        auto bin = forbiddenSourceRegister | RegisterBank::ToInstructionDestination(destination) << 3 | opcode;

        alu.Decode(bin, nullopt);
        // using 0x06 as source will lead to register indirect (thus using (HL));
        EXPECT_EQ(AddressingMode::RegisterIndirectSource, alu.InstructionData.AddressingMode);
    }
}

TEST(TestALU, DecodeRegisterIndirectAddressingMode)
{
    auto destinationsList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    auto rawBinary = static_cast<uint8_t>(0x00);
    ArithmeticLogicUnit alu;

    for (auto destination : destinationsList)
    {
        rawBinary = BinaryRegisterIndirectAsSourceAddressingMode(destination);
        alu.Decode(rawBinary, nullopt);

        EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
        EXPECT_EQ(AddressingMode::RegisterIndirectSource, alu.InstructionData.AddressingMode);
        EXPECT_EQ(Register::HL, alu.InstructionData.SourceRegister);
        EXPECT_EQ(destination, alu.InstructionData.DestinationRegister);
    }

    // (BC) and (DE) as sources -> Only A is accepted as source
    rawBinary = 0x0A; // Ld A, (BC)
    alu.Decode(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSource, alu.InstructionData.AddressingMode);
    EXPECT_EQ(Register::BC, alu.InstructionData.SourceRegister);
    EXPECT_EQ(Register::A, alu.InstructionData.DestinationRegister);

    rawBinary = 0x1A; // Ld A, (DE)
    alu.Decode(rawBinary, nullopt);
    
    EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSource, alu.InstructionData.AddressingMode);
    EXPECT_EQ(Register::DE, alu.InstructionData.SourceRegister);
    EXPECT_EQ(Register::A, alu.InstructionData.DestinationRegister);

    auto sourceList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};

    for (auto source : sourceList)
    {
        rawBinary = BinaryRegisterIndirectAsDestinationAddressingMode(source);
        alu.Decode(rawBinary, nullopt);

        EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
        EXPECT_EQ(AddressingMode::RegisterIndirectDestination, alu.InstructionData.AddressingMode);
        EXPECT_EQ(source, alu.InstructionData.SourceRegister);
        EXPECT_EQ(Register::HL, alu.InstructionData.DestinationRegister);
    }

    // (BC) and (DE) as sources -> Only A is accepted as source
    rawBinary = 0x02; // Ld (BC), A
    alu.Decode(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectDestination, alu.InstructionData.AddressingMode);
    EXPECT_EQ(Register::A, alu.InstructionData.SourceRegister);
    EXPECT_EQ(Register::BC, alu.InstructionData.DestinationRegister);

    rawBinary = 0x12; // Ld (DE), A
    alu.Decode(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectDestination, alu.InstructionData.AddressingMode);
    EXPECT_EQ(Register::A, alu.InstructionData.SourceRegister);
    EXPECT_EQ(Register::DE, alu.InstructionData.DestinationRegister);
}

TEST(TestALU, DecodeRegisterIndexedAddressingMode)
{
    return ;
    auto destinationsList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    auto preOpcode = static_cast<uint8_t>(0xDD);
    ArithmeticLogicUnit alu;

    for (auto destination : destinationsList)
    {
        auto rawBinary = BinaryRegisterIndexedSourceAddressingMode(destination);
        alu.Decode(rawBinary, preOpcode);

        EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
        EXPECT_EQ(AddressingMode::RegisterIndexedSource, alu.InstructionData.AddressingMode);
        EXPECT_EQ(Register::IX, alu.InstructionData.SourceRegister);
        EXPECT_EQ(destination, alu.InstructionData.DestinationRegister);
    }

    preOpcode = static_cast<uint8_t>(0xFD);

    for (auto destination : destinationsList)
    {
        auto rawBinary = BinaryRegisterIndexedSourceAddressingMode(destination);
        alu.Decode(rawBinary, preOpcode);

        EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
        EXPECT_EQ(AddressingMode::RegisterIndexedSource, alu.InstructionData.AddressingMode);
        EXPECT_EQ(Register::IY, alu.InstructionData.SourceRegister);
        EXPECT_EQ(destination, alu.InstructionData.DestinationRegister);
    }

    auto sourceList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};

    preOpcode = static_cast<uint8_t>(0xDD);
    for (auto source : sourceList)
    {
        auto rawBinary = BinaryRegisterIndexedDestinationAddressingMode(source);
        alu.Decode(rawBinary, preOpcode);

        EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
        EXPECT_EQ(AddressingMode::RegisterIndexedDestination, alu.InstructionData.AddressingMode);
        EXPECT_EQ(source, alu.InstructionData.SourceRegister);
        EXPECT_EQ(Register::IX, alu.InstructionData.DestinationRegister);
    }

    preOpcode = static_cast<uint8_t>(0xFD);
    for (auto source : sourceList)
    {
        auto rawBinary = BinaryRegisterIndexedDestinationAddressingMode(source);
        alu.Decode(rawBinary, preOpcode);

        EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
        EXPECT_EQ(AddressingMode::RegisterIndexedDestination, alu.InstructionData.AddressingMode);
        EXPECT_EQ(source, alu.InstructionData.SourceRegister);
        EXPECT_EQ(Register::IY, alu.InstructionData.DestinationRegister);
    }
}

TEST(TestALU, DecodeExtendedAddressingMode)
{
    ArithmeticLogicUnit alu;
    auto rawBinary = 0xFA;
    alu.Decode(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
    EXPECT_EQ(AddressingMode::ExtendedSource, alu.InstructionData.AddressingMode);
    EXPECT_EQ(Register::NoRegiser, alu.InstructionData.SourceRegister);
    EXPECT_EQ(Register::A, alu.InstructionData.DestinationRegister);

    rawBinary = 0xEA;
    alu.Decode(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
    EXPECT_EQ(AddressingMode::ExtendedDestination, alu.InstructionData.AddressingMode);
    EXPECT_EQ(Register::A, alu.InstructionData.SourceRegister);
    EXPECT_EQ(Register::NoRegiser, alu.InstructionData.DestinationRegister);
}


TEST(TestALU, DecodeImmediateRegisterIndirect)
{
    ArithmeticLogicUnit alu;
    auto rawBinary = 0x36;
    alu.Decode(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
    EXPECT_EQ(AddressingMode::ImmediateRegisterIndirect, alu.InstructionData.AddressingMode);
    EXPECT_EQ(Register::NoRegiser, alu.InstructionData.SourceRegister);
    EXPECT_EQ(Register::HL, alu.InstructionData.DestinationRegister);
}

TEST(TestALU, DecodeRegisterIndirectSourceIncrementAndDecrement)
{
    ArithmeticLogicUnit alu;
    auto rawBinary = 0x2A;
    alu.Decode(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSourceIncrement, alu.InstructionData.AddressingMode);
    EXPECT_EQ(Register::HL, alu.InstructionData.SourceRegister);
    EXPECT_EQ(Register::A, alu.InstructionData.DestinationRegister);

    rawBinary = 0x3A;
    alu.Decode(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSourceDecrement, alu.InstructionData.AddressingMode);
    EXPECT_EQ(Register::HL, alu.InstructionData.SourceRegister);
    EXPECT_EQ(Register::A, alu.InstructionData.DestinationRegister);
}

TEST(TestALU, DecodeRegisterIndirectDestinationIncrementAndDecrement)
{
    ArithmeticLogicUnit alu;
    auto rawBinary = 0x22;
    alu.Decode(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectDestinationIncrement, alu.InstructionData.AddressingMode);
    EXPECT_EQ(Register::A, alu.InstructionData.SourceRegister);
    EXPECT_EQ(Register::HL, alu.InstructionData.DestinationRegister);

    rawBinary = 0x32;
    alu.Decode(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectDestinationDecrement, alu.InstructionData.AddressingMode);
    EXPECT_EQ(Register::A, alu.InstructionData.SourceRegister);
    EXPECT_EQ(Register::HL, alu.InstructionData.DestinationRegister);
}

TEST(TestALU, DecodeRegisterImplicitAddressingMode)
{
    ArithmeticLogicUnit alu;
    auto rawBinary = 0xF2;
    alu.Decode(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
    EXPECT_EQ(AddressingMode::RegisterImplicitSource, alu.InstructionData.AddressingMode);
    EXPECT_EQ(Register::C, alu.InstructionData.SourceRegister);
    EXPECT_EQ(Register::A, alu.InstructionData.DestinationRegister);

    rawBinary = 0xE2;
    alu.Decode(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
    EXPECT_EQ(AddressingMode::RegisterImplicitDestination, alu.InstructionData.AddressingMode);
    EXPECT_EQ(Register::A, alu.InstructionData.SourceRegister);
    EXPECT_EQ(Register::C, alu.InstructionData.DestinationRegister);
}

TEST(TestALU, DecodeImmediateImplicitAddressingMode)
{
    ArithmeticLogicUnit alu;
    auto rawBinary = 0xF0;
    alu.Decode(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
    EXPECT_EQ(AddressingMode::ImmediateImplicitSource, alu.InstructionData.AddressingMode);
    EXPECT_EQ(Register::NoRegiser, alu.InstructionData.SourceRegister);
    EXPECT_EQ(Register::A, alu.InstructionData.DestinationRegister);

    rawBinary = 0xE0;
    alu.Decode(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
    EXPECT_EQ(AddressingMode::ImmediateImplicitDestination, alu.InstructionData.AddressingMode);
    EXPECT_EQ(Register::A, alu.InstructionData.SourceRegister);
    EXPECT_EQ(Register::NoRegiser, alu.InstructionData.DestinationRegister);
}

TEST(TestALU, DecodeImmediateRegisterPairAddressingMode)
{
    auto destinationList = {Register::BC, 
                            Register::DE, 
                            Register::HL, 
                            Register::SP};

    for (auto destination : destinationList)
    {
        ArithmeticLogicUnit alu;
        auto rawBinary = BinaryImmediateRegisterPair(destination);
        alu.Decode(rawBinary, nullopt);

        EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
        EXPECT_EQ(AddressingMode::ImmediatePair, alu.InstructionData.AddressingMode);
        EXPECT_EQ(Register::NoRegiser, alu.InstructionData.SourceRegister);
        EXPECT_EQ(destination, alu.InstructionData.DestinationRegister);
    }
}

TEST(TestALU, DecodeTransferToSP)
{
    ArithmeticLogicUnit alu;
    auto rawBinary = 0xF9;
    alu.Decode(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::ld, alu.InstructionData.Opcode);
    EXPECT_EQ(AddressingMode::RegisterPair, alu.InstructionData.AddressingMode);
    EXPECT_EQ(Register::HL, alu.InstructionData.SourceRegister);
    EXPECT_EQ(Register::SP, alu.InstructionData.DestinationRegister);
}

TEST(TestALU, DecodeAddRegisterMode)
{
    ArithmeticLogicUnit alu;
    auto binaryBase = 0x80;
    auto sourceList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};

    for (auto source : sourceList)
    {
        auto rawBinary = binaryBase | RegisterBank::ToInstructionSource(source);
        alu.Decode(rawBinary, nullopt);

        EXPECT_EQ(OpcodeType::add, alu.InstructionData.Opcode);
        EXPECT_EQ(AddressingMode::Register, alu.InstructionData.AddressingMode);
        EXPECT_EQ(source, alu.InstructionData.SourceRegister);
        EXPECT_EQ(Register::A, alu.InstructionData.DestinationRegister);
    }
}

TEST(TestALU, DecodeAddImmediateMode)
{
    ArithmeticLogicUnit alu;

    auto rawBinary = 0xC6;
    alu.Decode(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::add, alu.InstructionData.Opcode);
    EXPECT_EQ(AddressingMode::Immediate, alu.InstructionData.AddressingMode);
    EXPECT_EQ(Register::NoRegiser, alu.InstructionData.SourceRegister);
    EXPECT_EQ(Register::A, alu.InstructionData.DestinationRegister);
}

TEST(TestALU, ExecuteImmediateAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();
    auto memoryContent = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11};

    auto destinationsList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    for (auto i = static_cast<size_t>(0); i < destinationsList.size(); i++)
    {
        ArithmeticLogicUnit alu;
        auto rawBinary = BinaryImmediateAddressingMode(*(begin(destinationsList) + i));
        alu.Decode(rawBinary, nullopt);

        // simulate ALU acquiring operand from memory.
        alu.InstructionData.MemoryOperand1 = *(begin(memoryContent) + i);
        
        alu.Execute(registerBank);

        EXPECT_EQ(*(begin(memoryContent) + i), registerBank->Read(*(begin(destinationsList) + i)));
    }
}

TEST(TestALU, ExecuteRegisterAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();
    auto registersContent = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11};

    auto sourcesList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    auto destinationsList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    auto registerCounter = 0;
    for (auto source : sourcesList)
    {
        auto sourceValue = *(begin(registersContent) + (registerCounter++));
        registerBank->Write(source, sourceValue);
        
        for (auto destination : destinationsList)
            if (destination != source)
                registerBank->Write(destination, 0x00);

        for (auto destination : destinationsList)
        {
            ArithmeticLogicUnit alu;
            auto rawBinary = BinaryRegisterAddressingMode(source, destination);
            alu.Decode(rawBinary, nullopt);
            alu.Execute(registerBank);

            EXPECT_EQ(sourceValue, registerBank->Read(destination));
        }

        EXPECT_EQ(sourceValue, registerBank->Read(source));
    }
}

TEST(TestALU, ExecuteRegisterIndirectSourceAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();
    auto memoryContent = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11};
    auto destinationsList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    for (auto i = static_cast<size_t>(0); i < destinationsList.size(); i++)
    {
        ArithmeticLogicUnit alu;
        auto rawBinary = BinaryRegisterIndirectAsSourceAddressingMode(*(begin(destinationsList) + i));
        alu.Decode(rawBinary, nullopt);

        // simulate ALU acquiring operand from memory.
        alu.InstructionData.MemoryOperand1 = *(begin(memoryContent) + i);
        
        alu.Execute(registerBank);

        EXPECT_EQ(*(begin(memoryContent) + i), registerBank->Read(*(begin(destinationsList) + i)));
    }
}

TEST(TestALU, ExecuteRegisterIndirectDestinationAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();
    auto registerContent = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11};

    auto sourceList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    for (auto i = static_cast<size_t>(0); i < sourceList.size(); i++)
    {
        ArithmeticLogicUnit alu;
        auto rawBinary = BinaryRegisterIndirectAsDestinationAddressingMode(*(begin(sourceList) + i));
        alu.Decode(rawBinary, nullopt);

        registerBank->Write(*(begin(sourceList) + i), *(begin(registerContent) + i));
        alu.Execute(registerBank);

        EXPECT_EQ(*(begin(registerContent) + i), alu.InstructionData.MemoryResult1);
    }
}

TEST(TestALU, ExecuteRegisterIndexSourceAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();
    auto memoryContent = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11};
    auto destinationsList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    for (auto i = static_cast<size_t>(0); i < destinationsList.size(); i++)
    {
         ArithmeticLogicUnit alu;
        auto rawBinary = BinaryRegisterIndexedSourceAddressingMode(*(begin(destinationsList) + i));
        alu.Decode(rawBinary, 0xDD);

        // simulate ALU acquiring operand from memory.
        alu.InstructionData.MemoryOperand2 = *(begin(memoryContent) + i);
        
        alu.Execute(registerBank);

        EXPECT_EQ(*(begin(memoryContent) + i), registerBank->Read(*(begin(destinationsList) + i)));
    }
}

TEST(TestALU, ExecuteRegisterIndexDestinationAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();
    auto registerContent = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11};

    auto sourceList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    for (auto i = static_cast<size_t>(0); i < sourceList.size(); i++)
    {
        ArithmeticLogicUnit alu;
        auto rawBinary = BinaryRegisterIndexedDestinationAddressingMode(*(begin(sourceList) + i));
        alu.Decode(rawBinary, nullopt);

        registerBank->Write(*(begin(sourceList) + i), *(begin(registerContent) + i));
        alu.Execute(registerBank);

        EXPECT_EQ(*(begin(registerContent) + i), alu.InstructionData.MemoryResult1);
    }
}

TEST(TestALU, ExecuteExtendedSourceAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();

    ArithmeticLogicUnit alu;
    auto rawBinary = 0xFA;
    alu.Decode(rawBinary, nullopt);

    alu.InstructionData.MemoryOperand3 = 0xCC;
    alu.Execute(registerBank);

    EXPECT_EQ(0xCC, registerBank->Read(Register::A));

    rawBinary = 0xEA;
    alu.Decode(rawBinary, nullopt);

    registerBank->Write(Register::A, 0xEE);
    alu.Execute(registerBank);

    EXPECT_EQ(0xEE, alu.InstructionData.MemoryResult1);
}

TEST(TestALU, ExecuteImmediateRegisterIndirectAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();

    ArithmeticLogicUnit alu;
    auto rawBinary = 0x36;
    alu.Decode(rawBinary, nullopt);

    alu.InstructionData.MemoryOperand1 = 0xD0;
    alu.Execute(registerBank);

    EXPECT_EQ(0xD0, alu.InstructionData.MemoryResult1);
}

TEST(TestALU, ExecuteRegisterIndirectSourceIncrementAndDecrement)
{
    auto registerBank = make_shared<RegisterBank>();

    ArithmeticLogicUnit alu;
    auto rawBinary = 0x2A;
    alu.Decode(rawBinary, nullopt);

    alu.InstructionData.MemoryOperand1 = 0xCA;
    alu.Execute(registerBank);

    EXPECT_EQ(0xCA, registerBank->Read(Register::A));

    rawBinary = 0x3A;
    alu.Decode(rawBinary, nullopt);

    alu.InstructionData.MemoryOperand1 = 0x10;
    alu.Execute(registerBank);

    EXPECT_EQ(0x10, registerBank->Read(Register::A));
}

TEST(TestALU, ExecuteRegisterIndirectDestinationIncrementAndDecrement)
{
    auto registerBank = make_shared<RegisterBank>();

    ArithmeticLogicUnit alu;
    auto rawBinary = 0x22;
    alu.Decode(rawBinary, nullopt);

    registerBank->Write(Register::A, 0x77);
    alu.Execute(registerBank);

    EXPECT_EQ(0x77, alu.InstructionData.MemoryResult1);

    rawBinary = 0x32;
    alu.Decode(rawBinary, nullopt);

     registerBank->Write(Register::A, 0xA6);
    alu.Execute(registerBank);

    EXPECT_EQ(0xA6, alu.InstructionData.MemoryResult1);
}

TEST(TestALU, ExecuteImplicitRegisterAddresingMode)
{
    auto registerBank = make_shared<RegisterBank>();

    ArithmeticLogicUnit alu;
    auto rawBinary = 0xF2;
    alu.Decode(rawBinary, nullopt);

    alu.InstructionData.MemoryOperand1 = 0x6D;
    alu.Execute(registerBank);

    EXPECT_EQ(0x6D, registerBank->Read(Register::A));

    rawBinary = 0xE2;
    alu.Decode(rawBinary, nullopt);

    registerBank->Write(Register::A, 0xA1);
    alu.Execute(registerBank);

    EXPECT_EQ(0xA1, alu.InstructionData.MemoryResult1);
}

TEST(TestALU, ExecuteImplicitImmediateAddresingMode)
{
    auto registerBank = make_shared<RegisterBank>();

    ArithmeticLogicUnit alu;
    auto rawBinary = 0xF0;
    alu.Decode(rawBinary, nullopt);

    alu.InstructionData.MemoryOperand2 = 0x4A;
    alu.Execute(registerBank);

    EXPECT_EQ(0x4A, registerBank->Read(Register::A));

    rawBinary = 0xE0;
    alu.Decode(rawBinary, nullopt);

    registerBank->Write(Register::A, 0x78);
    alu.Execute(registerBank);

    EXPECT_EQ(0x78, alu.InstructionData.MemoryResult1);
}

TEST(TestALU, ExecuteImmediateRegisrerPairAddresingMode)
{
    auto destinationList = {Register::BC, 
                            Register::DE, 
                            Register::HL, 
                            Register::SP};

    auto immediateValues = {static_cast<uint16_t>(0x45AD),
                            static_cast<uint16_t>(0xF408),
                            static_cast<uint16_t>(0x21A6),
                            static_cast<uint16_t>(0x8BC1)};

    auto registerBank = make_shared<RegisterBank>();

    for (auto i = static_cast<size_t>(0); i < destinationList.size(); i++)
    {
        ArithmeticLogicUnit alu;
        auto rawBinary = BinaryImmediateRegisterPair(*(begin(destinationList) + i));
        alu.Decode(rawBinary, nullopt);

        alu.InstructionData.MemoryOperand1 = *(begin(immediateValues) + i) & 0xFF;
        alu.InstructionData.MemoryOperand2 = (*(begin(immediateValues) + i) >> 8) & 0xFF;

        alu.Execute(registerBank);

        EXPECT_EQ(*(begin(immediateValues) + i), registerBank->ReadPair(*(begin(destinationList) + i)));
    }
}

TEST(TestALU, ExecuteSPTransferAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();

    ArithmeticLogicUnit alu;
    auto rawBinary = 0xF9;
    alu.Decode(rawBinary, nullopt);

    registerBank->WritePair(Register::HL, 0x97A3);

    alu.Execute(registerBank);

    EXPECT_EQ(0x97A3, registerBank->ReadPair(Register::SP));
}


TEST(TestALU, ExecuteAddRegisterMode)
{
    auto registerBank = make_shared<RegisterBank>();
    auto sourceList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    ArithmeticLogicUnit alu;
    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<int8_t> distribution{-100, 100};
    vector<int8_t> registerAValues;
    vector<int8_t> registerSourceValues;

    for (auto i = 0; i < 7; i++)
        registerAValues.push_back(distribution(engine));

    for (auto i = 0; i < 7; i++)
        registerSourceValues.push_back(distribution(engine));

    auto counter = static_cast<size_t>(0);
    for (auto source : sourceList)
    {
        auto aValue = registerAValues[counter];
        auto sourceValue = registerSourceValues[counter];
        auto result = static_cast<uint8_t>(0);

         if(source != Register::A)
         {
            result = aValue + sourceValue;
            registerBank->Write(Register::A, aValue);
            registerBank->Write(source, sourceValue);
         }
         else
         {
            result = aValue + aValue;
            registerBank->Write(Register::A, aValue);
         }

        auto rawBinary = 0x80 | RegisterBank::ToInstructionSource(source);
        alu.Decode(rawBinary, nullopt);
        alu.Execute(registerBank);

        EXPECT_EQ(registerBank->Read(Register::A), static_cast<uint8_t>(result));
        counter++;
    }

    // Test Flags
    registerBank->Write(Register::A, 0xFF);
    registerBank->Write(Register::B, 0x01);

    auto rawBinary = 0x80 | RegisterBank::ToInstructionSource(Register::B);

    alu.Decode(rawBinary, nullopt);
    alu.Execute(registerBank);

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank->Read(Register::A));

    registerBank->Write(Register::A, 0x0F);
    registerBank->Write(Register::C, 0x01);

    rawBinary = 0x80 | RegisterBank::ToInstructionSource(Register::C);

    alu.Decode(rawBinary, nullopt);
    alu.Execute(registerBank);

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x10, registerBank->Read(Register::A));

    registerBank->Write(Register::A, 0xF0);
    registerBank->Write(Register::D, 0x80);

    rawBinary = 0x80 | RegisterBank::ToInstructionSource(Register::D);

    alu.Decode(rawBinary, nullopt);
    alu.Execute(registerBank);

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x70, registerBank->Read(Register::A));

    registerBank->Write(Register::A, 0x00);
    registerBank->Write(Register::E, 0x00);

    rawBinary = 0x80 | RegisterBank::ToInstructionSource(Register::E);

    alu.Decode(rawBinary, nullopt);
    alu.Execute(registerBank);

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank->Read(Register::A));

    registerBank->Write(Register::A, 0x01);
    registerBank->Write(Register::H, 0x01);

    rawBinary = 0x80 | RegisterBank::ToInstructionSource(Register::H);

    alu.Decode(rawBinary, nullopt);
    alu.Execute(registerBank);

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x02, registerBank->Read(Register::A));
}

TEST(TestALU, ExecuteAddImmediateMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicUnit alu;
    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<int8_t> distribution{-100, 100};
    vector<int8_t> registerAValues;
    vector<int8_t> memoryOperandValues;

    for (auto i = 0; i < 7; i++)
        registerAValues.push_back(distribution(engine));

    for (auto i = 0; i < 7; i++)
        memoryOperandValues.push_back(distribution(engine));

    for (auto i = 0; i < 7; i++)
    {
        auto aValue = registerAValues[i];
        auto memoryOperandValue = memoryOperandValues[i];
        auto result = static_cast<uint8_t>(0);

        result = aValue + memoryOperandValue;

        auto rawBinary = 0xC6;
        alu.Decode(rawBinary, nullopt);
        registerBank->Write(Register::A, aValue);
        alu.InstructionData.MemoryOperand1 = memoryOperandValue;
        alu.Execute(registerBank);

        EXPECT_EQ(registerBank->Read(Register::A), static_cast<uint8_t>(result));
    }

    // Test Flaga
    auto rawBinary = 0xC6;

    alu.Decode(rawBinary, nullopt);
    registerBank->Write(Register::A, 0xFE);
    alu.InstructionData.MemoryOperand1 = 0x02;
    alu.Execute(registerBank);

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank->Read(Register::A));

    alu.Decode(rawBinary, nullopt);
    registerBank->Write(Register::A, 0x0F);
    alu.InstructionData.MemoryOperand1 = 0x01;
    alu.Execute(registerBank);

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x10, registerBank->Read(Register::A));

    alu.Decode(rawBinary, nullopt);
    registerBank->Write(Register::A, 0xF0);
    alu.InstructionData.MemoryOperand1 = 0x80;
    alu.Execute(registerBank);

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x70, registerBank->Read(Register::A));

    alu.Decode(rawBinary, nullopt);
    registerBank->Write(Register::A, 0x00);
    alu.InstructionData.MemoryOperand1 = 0x00;
    alu.Execute(registerBank);

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank->Read(Register::A));

    alu.Decode(rawBinary, nullopt);
    registerBank->Write(Register::A, 0xFE);
    alu.InstructionData.MemoryOperand1 = 0x01;
    alu.Execute(registerBank);

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0xFF, registerBank->Read(Register::A));

}