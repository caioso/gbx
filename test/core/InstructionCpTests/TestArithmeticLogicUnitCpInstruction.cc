#include <gtest/gtest.h>

#include "CoreTestMocksAndWrappers.h"

#include <algorithm>
#include <memory>
#include <optional>
#include <random>
#include <vector>

#include <iostream>

#include "MemoryController.h"
#include "RegisterBank.h"
#include "ArithmeticLogicUnit.h"
#include "Opcodes.h"

using namespace std;
using namespace gbxcore;
using namespace gbxcore::interfaces;
using namespace gbxcore::instructions;


TEST(TestCp, DecodeCpRegisterMode)
{
    RegisterBank registerBank;
    auto operandList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    for (auto operand : operandList)
    {
        auto rawBinary = 0xB8 | RegisterBank::ToInstructionSource(operand);
        alu.DecodeInstruction(rawBinary, nullopt);

        EXPECT_EQ(OpcodeType::cp, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(operand, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(TestCp, ExecuteCpRegisterMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    // Test Flags
    auto rawBinary = 0xB8 | RegisterBank::ToInstructionSource(Register::D);

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x01);
    registerBank.Write(Register::D, 0x01);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x01, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x10);
    registerBank.Write(Register::D, 0x01);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x10, registerBank.Read(Register::A));
    
    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x01);
    registerBank.Write(Register::D, 0x02);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x01, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x01);
    registerBank.Write(Register::D, 0xFF);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x01, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x10);
    registerBank.Write(Register::D, 0x20);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x10, registerBank.Read(Register::A));
}

TEST(TestCp, DecodeCpImmediateMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto rawBinary = 0xFE;
    alu.DecodeInstruction(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::cp, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::Immediate, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
}

TEST(TestCp, ExecuteCpImmediateMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    // Test Flags
    auto rawBinary = 0xFE;

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0xFF);
    alu.GetInstructionData().MemoryOperand1 = 0xFF;
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0xFF, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x50);
    alu.GetInstructionData().MemoryOperand1 = 0x20;
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x50, registerBank.Read(Register::A));
    
    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x20);
    alu.GetInstructionData().MemoryOperand1 = 0x50;
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x20, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0xFE);
    alu.GetInstructionData().MemoryOperand1 = 0xFF;
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0xFE, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x00);
    alu.GetInstructionData().MemoryOperand1 = 0x01;
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank.Read(Register::A));
}

TEST(TestCp, DecodeCpRegisterIndirectMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto rawBinary = 0xBE;
    alu.DecodeInstruction(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::cp, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSource, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
}


TEST(TestCp, ExecuteCpRegisterIndirectMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    // Test Flags
    auto rawBinary = 0xBE;

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x00);
    alu.GetInstructionData().MemoryOperand1 = 0x00;
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x01);
    alu.GetInstructionData().MemoryOperand1 = 0x00;
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x01, registerBank.Read(Register::A));
    
    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0xA0);
    alu.GetInstructionData().MemoryOperand1 = 0x9F;
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0xA0, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x80);
    alu.GetInstructionData().MemoryOperand1 = 0xA0;
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x80, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x88);
    alu.GetInstructionData().MemoryOperand1 = 0xAA;
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x88, registerBank.Read(Register::A));
}