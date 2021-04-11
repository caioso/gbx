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

TEST(TestRrcaRraRrcAndRr, DecodeRrcaRegisterMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto rawBinary = 0x0F;
    alu.DecodeInstruction(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::rrca, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::A, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
}

TEST(TestRrcaRraRrcAndRr, ExecuteRrcaRegisterMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> distribution{0, 255};

    for (auto i = 0; i < 1000; i++)
    {
        auto rawBinary = 0x0F;
        auto operandValue = distribution(engine);
        auto operandValueLSBit = static_cast<uint8_t>(operandValue & 0x01);
        auto result = static_cast<uint8_t>((operandValue >> 1) | (operandValueLSBit << 7));

        alu.DecodeInstruction(rawBinary, nullopt);
        registerBank.Write(Register::A, operandValue);

        alu.Execute();

        EXPECT_EQ(result, registerBank.Read(Register::A));

        // Test Flags
        EXPECT_EQ(static_cast<uint8_t>(operandValueLSBit), registerBank.ReadFlag(Flag::CY));
        EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
        EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::N));
        EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    }
}

TEST(TestRrcaRraRrcAndRr, DecodeRraRegisterMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto rawBinary = 0x1F;
    alu.DecodeInstruction(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::rra, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::A, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
}

TEST(TestRrcaRraRrcAndRr, ExecuteRraRegisterMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> distribution{0, 255};
    uniform_int_distribution<uint8_t> carryDistribution{0, 255};

    for (auto i = 0; i < 1000; i++)
    {
        auto rawBinary = 0x1F;
        auto operandValue = distribution(engine);
        auto carryValue = static_cast<uint8_t>(carryDistribution(engine) < 128 ? 0 : 1);
        auto operandValueLSBit = static_cast<uint8_t>(operandValue & 0x01);
        auto result = static_cast<uint8_t>((operandValue >> 1) | (carryValue << 7));

        alu.DecodeInstruction(rawBinary, nullopt);
        registerBank.Write(Register::A, operandValue);
        registerBank.WriteFlag(Flag::CY, carryValue);

        alu.Execute();

        EXPECT_EQ(result, registerBank.Read(Register::A));

        // Test Flags
        EXPECT_EQ(static_cast<uint8_t>(operandValueLSBit), registerBank.ReadFlag(Flag::CY));
        EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
        EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::N));
        EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    }
}

TEST(TestRrcaRraRrcAndRr, DecodeRrcRegisterMode)
{
    auto operandList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    for (auto operand : operandList)
    {
        auto preOpcode = 0xCB;
        auto opcode = 0x08 | RegisterBankInterface::ToInstructionSource(operand);
        alu.DecodeInstruction(opcode, preOpcode);

        EXPECT_EQ(OpcodeType::rrc, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(operand, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(operand, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(TestRrcaRraRrcAndRr, ExecuteRrcRegisterMode)
{
    auto operandList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> distribution{0, 255};
    uniform_int_distribution<uint8_t> operandDistribution{0, 6};


    for (auto i  = 0; i < 1000; i++)
    {
        auto randomOperand = *(begin(operandList) +operandDistribution(engine));
        auto randomOperandValue = distribution(engine);

        auto lSBit = randomOperandValue & 0x01;
        auto result = (randomOperandValue >> 1) | (lSBit << 7);

        auto preOpcode = 0xCB;
        auto opcode = 0x08 | RegisterBankInterface::ToInstructionSource(randomOperand);

        alu.DecodeInstruction(opcode, preOpcode);

        registerBank.Write(randomOperand, randomOperandValue);

        alu.Execute();

        EXPECT_EQ(result, registerBank.Read(randomOperand));
        EXPECT_EQ(lSBit, registerBank.ReadFlag(Flag::CY));
        EXPECT_EQ((result == 0? 0x01 : 0x00), registerBank.ReadFlag(Flag::Z));
        EXPECT_EQ(static_cast<uint8_t>(0x00), registerBank.ReadFlag(Flag::N));
        EXPECT_EQ(static_cast<uint8_t>(0x00), registerBank.ReadFlag(Flag::H));

    }
}

TEST(TestRrcaRraRrcAndRr, DecodeRrcRegisterIndirectMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xCB;
    auto opcode = 0x0E;
    alu.DecodeInstruction(opcode, preOpcode);

    EXPECT_EQ(OpcodeType::rrc, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSourceAndDestination, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().DestinationRegister);
}

TEST(TestRrcaRraRrcAndRr, ExecuteRrcRegisterIndirectMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> distribution{0, 255};

    for (auto i  = 0; i < 1000; i++)
    {
        auto randomOperandValue = distribution(engine);

        auto lSBit = randomOperandValue & 0x01;
        auto result = (randomOperandValue >> 1) | (lSBit << 7);

        auto preOpcode = 0xCB;
        auto opcode = 0x0E;

        alu.DecodeInstruction(opcode, preOpcode);

        alu.GetInstructionData().MemoryOperand1 = randomOperandValue;

        alu.Execute();

        EXPECT_EQ(result, alu.GetInstructionData().MemoryResult1);
        EXPECT_EQ(lSBit, registerBank.ReadFlag(Flag::CY));
        EXPECT_EQ((result == 0? 0x01 : 0x00), registerBank.ReadFlag(Flag::Z));
        EXPECT_EQ(static_cast<uint8_t>(0x00), registerBank.ReadFlag(Flag::N));
        EXPECT_EQ(static_cast<uint8_t>(0x00), registerBank.ReadFlag(Flag::H));

    }
}

TEST(TestRrcaRraRrcAndRr, DecodeRrRegisterMode)
{
    auto operandList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    for (auto operand : operandList)
    {
        auto preOpcode = 0xCB;
        auto opcode = 0x18 | RegisterBankInterface::ToInstructionSource(operand);
        
        alu.DecodeInstruction(opcode, preOpcode);

        EXPECT_EQ(OpcodeType::rr, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(operand, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(operand, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(TestRrcaRraRrcAndRr, ExecuteRrRegisterMode)
{
    auto operandList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> distribution{0, 255};
    uniform_int_distribution<uint8_t> carryDistribution{0, 255};
    uniform_int_distribution<uint8_t> operandDistribution{0, 6};


    for (auto i  = 0; i < 1000; i++)
    {
        auto randomOperand = *(begin(operandList) +operandDistribution(engine));
        auto randomOperandValue = distribution(engine);
        auto randomCarryValue = carryDistribution(engine) % 2;

        auto lSBit = randomOperandValue & 0x01;
        auto result = (randomOperandValue >> 1) | (randomCarryValue << 7);

        auto preOpcode = 0xCB;
        auto opcode = 0x18 | RegisterBankInterface::ToInstructionSource(randomOperand);

        alu.DecodeInstruction(opcode, preOpcode);

        registerBank.Write(randomOperand, randomOperandValue);
        registerBank.WriteFlag(Flag::CY, randomCarryValue);

        alu.Execute();

        EXPECT_EQ(result, registerBank.Read(randomOperand));
        EXPECT_EQ(lSBit, registerBank.ReadFlag(Flag::CY));
        EXPECT_EQ((result == 0? 0x01 : 0x00), registerBank.ReadFlag(Flag::Z));
        EXPECT_EQ(static_cast<uint8_t>(0x00), registerBank.ReadFlag(Flag::N));
        EXPECT_EQ(static_cast<uint8_t>(0x00), registerBank.ReadFlag(Flag::H));

    }
}

TEST(TestRrcaRraRrcAndRr, DecodeRrRegisterIndirectMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xCB;
    auto opcode = 0x1E;
    
    alu.DecodeInstruction(opcode, preOpcode);

    EXPECT_EQ(OpcodeType::rr, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSourceAndDestination, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().DestinationRegister);
}

TEST(TestRrcaRraRrcAndRr, ExecuteRrRegisterIndirectMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> distribution{0, 255};
    uniform_int_distribution<uint8_t> carryDistribution{0, 255};


    for (auto i  = 0; i < 1000; i++)
    {
        auto randomOperandValue = distribution(engine);
        auto randomCarryValue = static_cast<uint8_t>(carryDistribution(engine) % 2);
        auto lSBit = static_cast<uint8_t>(randomOperandValue & 0x01);
        auto result = static_cast<uint8_t>((randomOperandValue >> 1) | (randomCarryValue << 7));

        auto preOpcode = 0xCB;
        auto opcode = 0x1E;

        alu.DecodeInstruction(opcode, preOpcode);

        alu.GetInstructionData().MemoryOperand1 = randomOperandValue;
        registerBank.WriteFlag(Flag::CY, randomCarryValue);

        alu.Execute();

        EXPECT_EQ(result, alu.GetInstructionData().MemoryResult1);
        EXPECT_EQ(lSBit, registerBank.ReadFlag(Flag::CY));
        EXPECT_EQ((result == 0? 0x01 : 0x00), registerBank.ReadFlag(Flag::Z));
        EXPECT_EQ(static_cast<uint8_t>(0x00), registerBank.ReadFlag(Flag::N));
        EXPECT_EQ(static_cast<uint8_t>(0x00), registerBank.ReadFlag(Flag::H));

    }
}