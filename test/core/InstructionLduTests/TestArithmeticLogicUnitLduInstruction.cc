#include <gtest/gtest.h>

#include "CoreTestMocksAndWrappers.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <random>
#include <vector>

#include "ArithmeticLogicUnit.h"
#include "MemoryController.h"
#include "Opcodes.h"
#include "RegisterBank.h"

using namespace std;
using namespace gbxcore;
using namespace gbxcore::interfaces;
using namespace gbxcore::instructions;


uint8_t LDUBinaryRegisterAddressingMode(Register source, Register destination)
{
    return RegisterBankInterface::ToInstructionSource(source) | 
           RegisterBankInterface::ToInstructionDestination(destination) << 3 |
           0x01 << 6;
}

uint8_t LDUBinaryImmediateAddressingMode(Register destination)
{
    return 0x06 | 
           RegisterBankInterface::ToInstructionDestination(destination) << 3 |
           0x00 << 6;
}

uint8_t LDUBinaryRegisterIndirectAsSourceAddressingMode(Register destination)
{
    return 0x06 | 
           RegisterBankInterface::ToInstructionDestination(destination) << 3 |
           0x01 << 6;
}

uint8_t LDUBinaryRegisterIndirectAsDestinationAddressingMode(Register source)
{
    return RegisterBankInterface::ToInstructionSource(source) | 
           0x06 << 3 |
           0x01 << 6;
}

uint8_t LDUBinaryRegisterIndexedSourceAddressingMode(Register destination)
{
    return 0x06 | 
           RegisterBankInterface::ToInstructionDestination(destination) << 3 | 
           0x01 << 6;
}

uint8_t LDUBinaryRegisterIndexedDestinationAddressingMode(Register source)
{
    return RegisterBankInterface::ToInstructionSource(source) | 
           (0x07 << 4);
}

uint8_t LDUBinaryImmediateRegisterPair(Register source)
{
    return (RegisterBankInterface::ToInstructionRegisterPair(source) << 4) | 0x01;
}

TEST(TestLdu, LDUDecodeImmediateAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();
    auto destinationsList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    for (auto destination : destinationsList)
    {
        auto preOpcode = 0xFC;
        auto rawBinary = LDUBinaryImmediateAddressingMode(destination);
        alu.DecodeInstruction(rawBinary, preOpcode);

        EXPECT_EQ(OpcodeType::ldu, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::Immediate, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(destination, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(TestLdu, LDUDecodingRegisterAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();
    auto sourcesList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    auto destinationsList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};

    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();
    
    for (auto source : sourcesList)
    for (auto destination : destinationsList)
    {
        auto preOpcode = 0xFC;
        auto rawBinary = LDUBinaryRegisterAddressingMode(source, destination);
        alu.DecodeInstruction(rawBinary, preOpcode);

        EXPECT_EQ(OpcodeType::ldu, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(source, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(destination, alu.GetInstructionData().DestinationRegister);
    }

    for (auto destination : destinationsList)
    {
        constexpr auto opcode = 0x01 << 6;
        constexpr auto forbiddenSourceRegister = (0x06);
        auto bin = forbiddenSourceRegister | RegisterBank::ToInstructionDestination(destination) << 3 | opcode;

        auto preOpcode = 0xFC;
        alu.DecodeInstruction(bin, preOpcode);
        // using 0x06 as source will lead to register indirect (thus using (HL));
        EXPECT_EQ(AddressingMode::RegisterIndirectSource, alu.GetInstructionData().AddressingMode);
    }
}

TEST(TestLdu, LDUDecodeRegisterIndirectAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();
    auto destinationsList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    auto rawBinary = static_cast<uint8_t>(0x00);

    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    for (auto destination : destinationsList)
    {
        auto preOpcode = 0xFC;
        rawBinary = LDUBinaryRegisterIndirectAsSourceAddressingMode(destination);
        alu.DecodeInstruction(rawBinary, preOpcode);

        EXPECT_EQ(OpcodeType::ldu, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::RegisterIndirectSource, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(Register::HL, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(destination, alu.GetInstructionData().DestinationRegister);
    }

    // (BC) and (DE) as sources -> Only A is accepted as source
    auto preOpcode = 0xFC;
    rawBinary = 0x0A; // Ld A, (BC)
    alu.DecodeInstruction(rawBinary, preOpcode);

    EXPECT_EQ(OpcodeType::ldu, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSource, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::BC, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);

    rawBinary = 0x1A; // Ld A, (DE)
    preOpcode = 0xFC;
    alu.DecodeInstruction(rawBinary, preOpcode);
    
    EXPECT_EQ(OpcodeType::ldu, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSource, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::DE, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);

    auto sourceList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};

    for (auto source : sourceList)
    {
        rawBinary = LDUBinaryRegisterIndirectAsDestinationAddressingMode(source);
        preOpcode = 0xFC;        
        alu.DecodeInstruction(rawBinary, preOpcode);

        EXPECT_EQ(OpcodeType::ldu, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::RegisterIndirectDestination, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(source, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(Register::HL, alu.GetInstructionData().DestinationRegister);
    }

    // (BC) and (DE) as sources -> Only A is accepted as source
    preOpcode = 0xFC;
    rawBinary = 0x02; // Ld (BC), A
    alu.DecodeInstruction(rawBinary, preOpcode);

    EXPECT_EQ(OpcodeType::ldu, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectDestination, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::A, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::BC, alu.GetInstructionData().DestinationRegister);

    preOpcode = 0xFC;
    rawBinary = 0x12; // Ld (DE), A
    alu.DecodeInstruction(rawBinary, preOpcode);

    EXPECT_EQ(OpcodeType::ldu, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectDestination, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::A, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::DE, alu.GetInstructionData().DestinationRegister);
}

TEST(TestLdu, LDUDecodeExtendedAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xFC;
    auto rawBinary = 0xFA;
    alu.DecodeInstruction(rawBinary, preOpcode);

    EXPECT_EQ(OpcodeType::ldu, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::ExtendedSource, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);

    rawBinary = 0xEA;
    alu.DecodeInstruction(rawBinary, preOpcode);

    EXPECT_EQ(OpcodeType::ldu, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::ExtendedDestination, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::A, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().DestinationRegister);
}


TEST(TestLdu, LDUDecodeImmediateRegisterIndirect)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xFC;
    auto rawBinary = 0x36;
    alu.DecodeInstruction(rawBinary, preOpcode);

    EXPECT_EQ(OpcodeType::ldu, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::ImmediateRegisterIndirect, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().DestinationRegister);
}

TEST(TestLdu, LDUDecodeRegisterIndirectSourceIncrementAndDecrement)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xFC;
    auto rawBinary = 0x2A;
    alu.DecodeInstruction(rawBinary, preOpcode);

    EXPECT_EQ(OpcodeType::ldu, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSourceIncrement, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);

    rawBinary = 0x3A;
    alu.DecodeInstruction(rawBinary, preOpcode);

    EXPECT_EQ(OpcodeType::ldu, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSourceDecrement, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
}

TEST(TestLdu, LDUDecodeRegisterIndirectDestinationIncrementAndDecrement)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xFC;
    auto rawBinary = 0x22;
    alu.DecodeInstruction(rawBinary, preOpcode);

    EXPECT_EQ(OpcodeType::ldu, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectDestinationIncrement, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::A, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().DestinationRegister);

    rawBinary = 0x32;
    alu.DecodeInstruction(rawBinary, preOpcode);

    EXPECT_EQ(OpcodeType::ldu, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectDestinationDecrement, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::A, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().DestinationRegister);
}

TEST(TestLdu, LDUDecodeRegisterImplicitAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xFC;
    auto rawBinary = 0xF2;
    alu.DecodeInstruction(rawBinary, preOpcode);

    EXPECT_EQ(OpcodeType::ldu, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterImplicitSource, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::C, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);

    rawBinary = 0xE2;
    alu.DecodeInstruction(rawBinary, preOpcode);

    EXPECT_EQ(OpcodeType::ldu, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterImplicitDestination, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::A, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::C, alu.GetInstructionData().DestinationRegister);
}

TEST(TestLdu, LDUDecodeImmediateImplicitAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xFC;
    auto rawBinary = 0xF0;
    alu.DecodeInstruction(rawBinary, preOpcode);

    EXPECT_EQ(OpcodeType::ldu, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::ImmediateImplicitSource, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);

    rawBinary = 0xE0;
    alu.DecodeInstruction(rawBinary, preOpcode);

    EXPECT_EQ(OpcodeType::ldu, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::ImmediateImplicitDestination, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::A, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().DestinationRegister);
}

TEST(TestLdu, LDUDecodeImmediateRegisterPairAddressingMode)
{
    auto destinationList = {Register::BC, 
                            Register::DE, 
                            Register::HL, 
                            Register::SP};

    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    for (auto destination : destinationList)
    {
        auto preOpcode = 0xFC;
        auto rawBinary = LDUBinaryImmediateRegisterPair(destination);
        alu.DecodeInstruction(rawBinary, preOpcode);

        EXPECT_EQ(OpcodeType::ldu, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::ImmediatePair, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(destination, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(TestLdu, LDUDecodeTransferToSP)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xFC;
    auto rawBinary = 0xF9;
    alu.DecodeInstruction(rawBinary, preOpcode);

    EXPECT_EQ(OpcodeType::ldu, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterPair, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::SP, alu.GetInstructionData().DestinationRegister);
}

TEST(TestLdu, LDUDecodeTransferSPToMemory)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xFC;
    auto rawBinary = 0x08;
    alu.DecodeInstruction(rawBinary, preOpcode);

    EXPECT_EQ(OpcodeType::ldu, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::ExtendedDestinationPair, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::SP, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().DestinationRegister);
}

TEST(TestLdu, LDUExecuteImmediateAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto memoryContent = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11};

    auto destinationsList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    for (auto i = static_cast<size_t>(0); i < destinationsList.size(); i++)
    {
        auto preOpcode = 0xFC;
        auto rawBinary = LDUBinaryImmediateAddressingMode(*(begin(destinationsList) + i));
        alu.DecodeInstruction(rawBinary, preOpcode);

        // simulate ALU acquiring operand from memory.
        alu.GetInstructionData().MemoryOperand1 = *(begin(memoryContent) + i);
        
        alu.Execute();

        EXPECT_EQ(*(begin(memoryContent) + i), registerBank->Read(*(begin(destinationsList) + i)));
    }
}

TEST(TestLdu, LDUExecuteRegisterAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();

    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

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
            auto preOpcode = 0xFC;
            auto rawBinary = LDUBinaryRegisterAddressingMode(source, destination);
            alu.DecodeInstruction(rawBinary, preOpcode);
            alu.Execute();

            EXPECT_EQ(sourceValue, registerBank->Read(destination));
        }

        EXPECT_EQ(sourceValue, registerBank->Read(source));
    }
}

TEST(TestLdu, LDUExecuteRegisterIndirectSourceAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();

    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto memoryContent = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11};
    auto destinationsList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    for (auto i = static_cast<size_t>(0); i < destinationsList.size(); i++)
    {
        auto preOpcode = 0xFC;
        auto rawBinary = LDUBinaryRegisterIndirectAsSourceAddressingMode(*(begin(destinationsList) + i));
        alu.DecodeInstruction(rawBinary, preOpcode);

        // simulate ALU acquiring operand from memory.
        alu.GetInstructionData().MemoryOperand1 = *(begin(memoryContent) + i);
        
        alu.Execute();

        EXPECT_EQ(*(begin(memoryContent) + i), registerBank->Read(*(begin(destinationsList) + i)));
    }
}

TEST(TestLdu, LDUExecuteRegisterIndirectDestinationAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();

    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto registerContent = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11};

    auto sourceList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    for (auto i = static_cast<size_t>(0); i < sourceList.size(); i++)
    {
        auto preOpcode = 0xFC;
        auto rawBinary = LDUBinaryRegisterIndirectAsDestinationAddressingMode(*(begin(sourceList) + i));
        alu.DecodeInstruction(rawBinary, preOpcode);

        registerBank->Write(*(begin(sourceList) + i), *(begin(registerContent) + i));
        alu.Execute();

        EXPECT_EQ(*(begin(registerContent) + i), alu.GetInstructionData().MemoryResult1);
    }
}

TEST(TestLdu, LDUExecuteRegisterIndexDestinationAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();

    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto registerContent = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11};

    auto sourceList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    for (auto i = static_cast<size_t>(0); i < sourceList.size(); i++)
    {
        auto preOpcode = 0xFC;
        auto rawBinary = LDUBinaryRegisterIndexedDestinationAddressingMode(*(begin(sourceList) + i));
        alu.DecodeInstruction(rawBinary, preOpcode);

        registerBank->Write(*(begin(sourceList) + i), *(begin(registerContent) + i));
        alu.Execute();

        EXPECT_EQ(*(begin(registerContent) + i), alu.GetInstructionData().MemoryResult1);
    }
}

TEST(TestLdu, LDUExecuteExtendedSourceAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();

    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xFC;
    auto rawBinary = 0xFA;
    alu.DecodeInstruction(rawBinary, preOpcode);

    alu.GetInstructionData().MemoryOperand3 = 0xCC;
    alu.Execute();

    EXPECT_EQ(0xCC, registerBank->Read(Register::A));

    rawBinary = 0xEA;
    alu.DecodeInstruction(rawBinary, preOpcode);

    registerBank->Write(Register::A, 0xEE);
    alu.Execute();

    EXPECT_EQ(0xEE, alu.GetInstructionData().MemoryResult1);
}

TEST(TestLdu, LDUExecuteImmediateRegisterIndirectAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();

    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xFC;
    auto rawBinary = 0x36;
    alu.DecodeInstruction(rawBinary, preOpcode);

    alu.GetInstructionData().MemoryOperand1 = 0xD0;
    alu.Execute();

    EXPECT_EQ(0xD0, alu.GetInstructionData().MemoryResult1);
}

TEST(TestLdu, LDUExecuteRegisterIndirectSourceIncrementAndDecrement)
{
    auto registerBank = make_shared<RegisterBank>();

    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xFC;
    auto rawBinary = 0x2A;
    alu.DecodeInstruction(rawBinary, preOpcode);

    alu.GetInstructionData().MemoryOperand1 = 0xCA;
    alu.Execute();

    EXPECT_EQ(0xCA, registerBank->Read(Register::A));

    rawBinary = 0x3A;
    alu.DecodeInstruction(rawBinary, preOpcode);

    alu.GetInstructionData().MemoryOperand1 = 0x10;
    alu.Execute();

    EXPECT_EQ(0x10, registerBank->Read(Register::A));
}

TEST(TestLdu, LDUExecuteRegisterIndirectDestinationIncrementAndDecrement)
{
    auto registerBank = make_shared<RegisterBank>();

    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xFC;
    auto rawBinary = 0x22;
    alu.DecodeInstruction(rawBinary, preOpcode);

    registerBank->Write(Register::A, 0x77);
    alu.Execute();

    EXPECT_EQ(0x77, alu.GetInstructionData().MemoryResult1);

    rawBinary = 0x32;
    alu.DecodeInstruction(rawBinary, preOpcode);

     registerBank->Write(Register::A, 0xA6);
    alu.Execute();

    EXPECT_EQ(0xA6, alu.GetInstructionData().MemoryResult1);
}

TEST(TestLdu, LDUExecuteImplicitRegisterAddresingMode)
{
    auto registerBank = make_shared<RegisterBank>();

    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xFC;
    auto rawBinary = 0xF2;
    alu.DecodeInstruction(rawBinary, preOpcode);

    alu.GetInstructionData().MemoryOperand1 = 0x6D;
    alu.Execute();

    EXPECT_EQ(0x6D, registerBank->Read(Register::A));

    rawBinary = 0xE2;
    alu.DecodeInstruction(rawBinary, preOpcode);

    registerBank->Write(Register::A, 0xA1);
    alu.Execute();

    EXPECT_EQ(0xA1, alu.GetInstructionData().MemoryResult1);
}

TEST(TestLdu, LDUExecuteImplicitImmediateAddresingMode)
{
    auto registerBank = make_shared<RegisterBank>();

    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xFC;
    auto rawBinary = 0xF0;
    alu.DecodeInstruction(rawBinary, preOpcode);

    alu.GetInstructionData().MemoryOperand2 = 0x4A;
    alu.Execute();

    EXPECT_EQ(0x4A, registerBank->Read(Register::A));

    rawBinary = 0xE0;
    alu.DecodeInstruction(rawBinary, preOpcode);

    registerBank->Write(Register::A, 0x78);
    alu.Execute();

    EXPECT_EQ(0x78, alu.GetInstructionData().MemoryResult1);
}

TEST(TestLdu, LDUExecuteImmediateRegisrerPairAddresingMode)
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

    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    for (auto i = static_cast<size_t>(0); i < destinationList.size(); i++)
    {
        auto preOpcode = 0xFC;
        auto rawBinary = LDUBinaryImmediateRegisterPair(*(begin(destinationList) + i));
        alu.DecodeInstruction(rawBinary, preOpcode);

        alu.GetInstructionData().MemoryOperand1 = *(begin(immediateValues) + i) & 0xFF;
        alu.GetInstructionData().MemoryOperand2 = (*(begin(immediateValues) + i) >> 8) & 0xFF;

        alu.Execute();

        EXPECT_EQ(*(begin(immediateValues) + i), registerBank->ReadPair(*(begin(destinationList) + i)));
    }
}

TEST(TestLdu, LDUExecuteSPTransferAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();

    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xFC;
    auto rawBinary = 0xF9;
    alu.DecodeInstruction(rawBinary, preOpcode);

    registerBank->WritePair(Register::HL, 0x97A3);

    alu.Execute();

    EXPECT_EQ(0x97A3, registerBank->ReadPair(Register::SP));
}

TEST(TestLdu, LDUExecuteSPTransferToMemoryMode)
{
    auto registerBank = make_shared<RegisterBank>();

    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xFC;
    auto rawBinary = 0x08;
    registerBank->WritePair(Register::SP, 0x54D1);
    alu.DecodeInstruction(rawBinary, preOpcode);

    alu.Execute();

    EXPECT_EQ(static_cast<uint8_t>(0xD1), alu.GetInstructionData().MemoryResult1);
    EXPECT_EQ(static_cast<uint8_t>(0x54), alu.GetInstructionData().MemoryResult2);
}