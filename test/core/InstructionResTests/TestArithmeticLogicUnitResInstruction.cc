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

TEST(CoreTests_TestRES, DecodeResRegisterMode)
{
    auto operandList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    for (uint8_t bitCounter = 0; bitCounter < 8; bitCounter++)
    {
        for (auto operand : operandList)
        {
            auto preOpcode = 0xCB;
            auto opcode = 0x80 | (bitCounter << 0x03) | RegisterBankInterface::ToInstructionSource(operand);
            alu.DecodeInstruction(opcode, preOpcode);

            EXPECT_EQ(OpcodeType::res, alu.GetInstructionData().Opcode);
            EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
            EXPECT_EQ(bitCounter, alu.GetInstructionData().InstructionExtraOperand);
            EXPECT_EQ(operand, alu.GetInstructionData().SourceRegister);
            EXPECT_EQ(operand, alu.GetInstructionData().DestinationRegister);
        }
    }
}

TEST(CoreTests_TestRES, ExecuteResRegisterMode)
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

    for (auto i = 0; i < 1000; i++)
    {
        auto operandValue = distribution(engine);
        auto operand = *(begin(operandList) + operandDistribution(engine));
        auto targetBit = distribution(engine) % 8;
        auto result = static_cast<uint8_t>(operandValue & (~(0x01 << targetBit)));

        auto preOpcode = 0xCB;
        auto opcode = 0x80 | (targetBit << 0x03) | RegisterBankInterface::ToInstructionSource(operand);
        alu.DecodeInstruction(opcode, preOpcode);

        registerBank.Write(operand, operandValue);

        auto cyValue = distribution(engine) % 2;
        auto zValue = distribution(engine) % 2;
        auto hValue = distribution(engine) % 2;
        auto nValue = distribution(engine) % 2;

        registerBank.WriteFlag(Flag::CY, cyValue);
        registerBank.WriteFlag(Flag::Z, zValue);
        registerBank.WriteFlag(Flag::H, hValue);
        registerBank.WriteFlag(Flag::N, nValue);

        alu.Execute();

        EXPECT_EQ(result, registerBank.Read(operand));
        EXPECT_EQ(cyValue, registerBank.ReadFlag(Flag::CY));
        EXPECT_EQ(zValue, registerBank.ReadFlag(Flag::Z));
        EXPECT_EQ(hValue, registerBank.ReadFlag(Flag::H));
        EXPECT_EQ(nValue, registerBank.ReadFlag(Flag::N));
    }
}

TEST(CoreTests_TestRES, DecodeResRegisterIndirectMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    for (uint8_t bitCounter = 0; bitCounter < 8; bitCounter++)
    {
        auto preOpcode = 0xCB;
        auto opcode = 0x80 | (bitCounter << 0x03) | 0x06;
        alu.DecodeInstruction(opcode, preOpcode);

        EXPECT_EQ(OpcodeType::res, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::RegisterIndirectSourceAndDestination, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(bitCounter, alu.GetInstructionData().InstructionExtraOperand);
        EXPECT_EQ(Register::HL, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(Register::HL, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(CoreTests_TestRES, ExecuteResRegisterIndirectMode)
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
        auto operandValue = distribution(engine);
        auto targetBit = distribution(engine) % 8;
        auto result = static_cast<uint8_t>(operandValue & (~(0x01 << targetBit)));

        auto preOpcode = 0xCB;
        auto opcode = 0x80 | (targetBit << 0x03) | 0x06;
        alu.DecodeInstruction(opcode, preOpcode);

        alu.GetInstructionData().MemoryOperand1 = operandValue;

        auto cyValue = distribution(engine) % 2;
        auto zValue = distribution(engine) % 2;
        auto hValue = distribution(engine) % 2;
        auto nValue = distribution(engine) % 2;

        registerBank.WriteFlag(Flag::CY, cyValue);
        registerBank.WriteFlag(Flag::Z, zValue);
        registerBank.WriteFlag(Flag::H, hValue);
        registerBank.WriteFlag(Flag::N, nValue);

        alu.Execute();

        EXPECT_EQ(result, alu.GetInstructionData().MemoryResult1);
        EXPECT_EQ(cyValue, registerBank.ReadFlag(Flag::CY));
        EXPECT_EQ(zValue, registerBank.ReadFlag(Flag::Z));
        EXPECT_EQ(hValue, registerBank.ReadFlag(Flag::H));
        EXPECT_EQ(nValue, registerBank.ReadFlag(Flag::N));
    }
}