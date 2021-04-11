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

TEST(TestRlcaRlaRlcAndRl, DecodeRlcaRegisterMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto rawBinary = 0x07;
    alu.DecodeInstruction(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::rlca, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::A, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
}

TEST(TestRlcaRlaRlcAndRl, ExecuteRlcaRegisterMode)
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
        auto rawBinary = 0x07;
        auto operandValue = distribution(engine);
        auto operandValueMSBit = static_cast<uint8_t>((operandValue >> 7) & 0x01);
        auto result = static_cast<uint8_t>((operandValue << 1) | operandValueMSBit);

        alu.DecodeInstruction(rawBinary, nullopt);
        registerBank.Write(Register::A, operandValue);

        alu.Execute();

        EXPECT_EQ(result, registerBank.Read(Register::A));

        // Test Flags
        EXPECT_EQ(static_cast<uint8_t>(operandValueMSBit), registerBank.ReadFlag(Flag::CY));
        EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
        EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::N));
        EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    }
}

TEST(TestRlcaRlaRlcAndRl, DecodeRla)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto rawBinary = 0x17;
    alu.DecodeInstruction(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::rla, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::A, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
}

TEST(TestRlcaRlaRlcAndRl, ExecuteRlaRegisterMode)
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
        auto rawBinary = 0x17;
        auto operandValue = distribution(engine);
        auto carryValue = static_cast<uint8_t>(carryDistribution(engine) < 128 ? 0 : 1);
        auto operandValueMSBit = static_cast<uint8_t>((operandValue >> 7) & 0x01);
        auto result = static_cast<uint8_t>((operandValue << 1) | carryValue);

        alu.DecodeInstruction(rawBinary, nullopt);
        registerBank.Write(Register::A, operandValue);
        registerBank.WriteFlag(Flag::CY, carryValue);

        alu.Execute();

        EXPECT_EQ(result, registerBank.Read(Register::A));

        // Test Flags
        EXPECT_EQ(operandValueMSBit, registerBank.ReadFlag(Flag::CY));
        EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
        EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::N));
        EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    }
}

TEST(TestRlcaRlaRlcAndRl, DecodeRlcRegisterMode)
{
    RegisterBank registerBank;
    auto operandList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};

    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xCB;

    for (auto operand : operandList)
    {
        auto opcode = RegisterBankInterface::ToInstructionSource(operand);
        alu.DecodeInstruction(opcode, preOpcode);

        EXPECT_EQ(OpcodeType::rlc, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(operand, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(operand, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(TestRlcaRlaRlcAndRl, ExecuteRlcRegisterMode)
{
    auto operandList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> distribution{0, 255};
    uniform_int_distribution<uint8_t> sourceDistribution{0, 6};

    for (auto i = 0; i < 1000; i++)
    {
        auto preOpcode = 0xCB;
        auto operandValue = distribution(engine);
        auto operand = *(begin(operandList) + sourceDistribution(engine));
        auto operandValueMSBit = static_cast<uint8_t>((operandValue >> 7) & 0x01);
        auto result = static_cast<uint8_t>((operandValue << 1) | operandValueMSBit);

        auto opcode = RegisterBankInterface::ToInstructionSource(operand);
        alu.DecodeInstruction(opcode, preOpcode);
        registerBank.Write(operand, operandValue);
        registerBank.WriteFlag(Flag::CY, operandValueMSBit);
        alu.Execute();

        EXPECT_EQ(result, registerBank.Read(operand));

        // Test Flags
        EXPECT_EQ(operandValueMSBit, registerBank.ReadFlag(Flag::CY));
        EXPECT_EQ((result == 0? 0x01 : 0x00), registerBank.ReadFlag(Flag::Z));
        EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
        EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::N));
    }
}

TEST(TestRlcaRlaRlcAndRl, DecodeRlcRegisterIndirect)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xCB;
    auto opcode = 0x06;
    alu.DecodeInstruction(opcode, preOpcode);

    EXPECT_EQ(OpcodeType::rlc, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSourceAndDestination, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().DestinationRegister);
}


TEST(TestRlcaRlaRlcAndRl, ExecuteRlcRegisterIndirectMode)
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
        auto preOpcode = 0xCB;
        auto operandValue = distribution(engine);
        auto operandValueMSBit = static_cast<uint8_t>((operandValue >> 7) & 0x01);
        auto result = static_cast<uint8_t>((operandValue << 1) | operandValueMSBit);

        auto opcode = 0x06;
        alu.DecodeInstruction(opcode, preOpcode);
        
        registerBank.WriteFlag(Flag::CY, operandValueMSBit);

        alu.GetInstructionData().MemoryOperand1 = operandValue;
        alu.Execute();

        EXPECT_EQ(result, alu.GetInstructionData().MemoryResult1);

        // Test Flags
        EXPECT_EQ(operandValueMSBit, registerBank.ReadFlag(Flag::CY));
        EXPECT_EQ((result == 0? 0x01 : 0x00), registerBank.ReadFlag(Flag::Z));
        EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
        EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::N));
    }
}

TEST(TestRlcaRlaRlcAndRl, DecodeRlRegisterMode)
{
    RegisterBank registerBank;
    auto operandList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};

    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xCB;

    for (auto operand : operandList)
    {
        auto opcode = 0x10 | RegisterBankInterface::ToInstructionSource(operand);
        alu.DecodeInstruction(opcode, preOpcode);

        EXPECT_EQ(OpcodeType::rl, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(operand, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(operand, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(TestRlcaRlaRlcAndRl, ExecuteRlRegisterMode)
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
    uniform_int_distribution<uint8_t> sourceDistribution{0, 6};

    for (auto i = 0; i < 1000; i++)
    {
        auto operand = *(begin(operandList) + sourceDistribution(engine));

        auto preOpcode = 0xCB;
        auto opcode = 0x10 | RegisterBankInterface::ToInstructionSource(operand);
        auto operandValue = distribution(engine);
        auto carryValue = static_cast<uint8_t>(carryDistribution(engine) < 128 ? 0 : 1);
        auto operandValueMSBit = static_cast<uint8_t>((operandValue >> 7) & 0x01);
        auto result = static_cast<uint8_t>((operandValue << 1) | carryValue);

        alu.DecodeInstruction(opcode, preOpcode);
        registerBank.Write(operand, operandValue);
        registerBank.WriteFlag(Flag::CY, carryValue);

        alu.Execute();

        EXPECT_EQ(result, registerBank.Read(operand));

        // Test Flags
        EXPECT_EQ(operandValueMSBit, registerBank.ReadFlag(Flag::CY));
        EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
        EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::N));
        EXPECT_EQ((result == 0? 0x01 : 0x00), registerBank.ReadFlag(Flag::Z));
    }
}


TEST(TestRlcaRlaRlcAndRl, DecodeRlRegisterIndirect)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xCB;
    auto opcode = 0x16;
    alu.DecodeInstruction(opcode, preOpcode);

    EXPECT_EQ(OpcodeType::rl, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSourceAndDestination, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().DestinationRegister);
}

TEST(TestRlcaRlaRlcAndRl, ExecuteRlRegisterIndirectMode)
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
        auto preOpcode = 0xCB;
        auto operandValue = distribution(engine);
        auto carryValue = static_cast<uint8_t>(carryDistribution(engine) < 128 ? 0 : 1);
        auto operandValueMSBit = static_cast<uint8_t>((operandValue >> 7) & 0x01);
        auto result = static_cast<uint8_t>((operandValue << 1) | carryValue);

        auto opcode = 0x16;
        alu.DecodeInstruction(opcode, preOpcode);
        
        registerBank.WriteFlag(Flag::CY, carryValue);

        alu.GetInstructionData().MemoryOperand1 = operandValue;
        alu.Execute();

        EXPECT_EQ(result, alu.GetInstructionData().MemoryResult1);

        // Test Flags
        EXPECT_EQ(operandValueMSBit, registerBank.ReadFlag(Flag::CY));
        EXPECT_EQ((result == 0? 0x01 : 0x00), registerBank.ReadFlag(Flag::Z));
        EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
        EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::N));
    }
}
