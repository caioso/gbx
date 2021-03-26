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

TEST(TestSwap, DecodeSwapRegisterMode)
{
    auto operandList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    for (auto operand : operandList)
    {
        auto preOpcode = 0xCB;
        auto opcode = 0x30 | RegisterBankInterface::ToInstructionSource(operand);
        alu.DecodeInstruction(opcode, preOpcode);

        EXPECT_EQ(OpcodeType::swap, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(operand, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(operand, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(TestSwap, ExecuteSwapRegisterMode)
{
    auto operandList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> distribution{0, 255};
    uniform_int_distribution<uint8_t> operandDistribution{0, 6};

    for (auto i = 0; i < 1000; i++)
    {
        auto operandValue = distribution(engine);
        auto operand = *(begin(operandList) + operandDistribution(engine));
        auto lowerNibble = static_cast<uint8_t>((operandValue) & 0x0F);
        auto result = static_cast<uint8_t>((operandValue >> 4) | (lowerNibble << 4));

        auto preOpcode = 0xCB;
        auto opcode = 0x30 | RegisterBankInterface::ToInstructionSource(operand);
        alu.DecodeInstruction(opcode, preOpcode);
        registerBank->Write(operand, operandValue);

        alu.Execute();

        EXPECT_EQ(result, registerBank->Read(operand));

        // Test Flags
        EXPECT_EQ((result == 0? 0x01 : 0x00), registerBank->ReadFlag(Flag::Z));
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    }
}

TEST(TestSwap, DecodeSwapRegisterIndirectMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xCB;
    auto opcode = 0x36;
    alu.DecodeInstruction(opcode, preOpcode);

    EXPECT_EQ(OpcodeType::swap, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSourceAndDestination, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().DestinationRegister);
}

TEST(TestSwap, ExecuteSrlRegisterIndirectMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> distribution{0, 255};
    uniform_int_distribution<uint8_t> operandDistribution{0, 6};

    for (auto i = 0; i < 1000; i++)
    {
        auto operandValue = distribution(engine);
        auto lowerNibble = static_cast<uint8_t>((operandValue) & 0x0F);
        auto result = static_cast<uint8_t>((operandValue >> 4) | (lowerNibble << 4));

        auto preOpcode = 0xCB;
        auto opcode = 0x36;
        
        alu.DecodeInstruction(opcode, preOpcode);
        alu.GetInstructionData().MemoryOperand1 = operandValue;

        alu.Execute();

        EXPECT_EQ(result, alu.GetInstructionData().MemoryResult1);

        // Test Flags
        EXPECT_EQ((result == 0? 0x01 : 0x00), registerBank->ReadFlag(Flag::Z));
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    }
}