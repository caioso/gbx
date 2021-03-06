#include <gtest/gtest.h>

#include "CoreTestMocksAndWrappers.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <random>
#include <vector>

#include "MemoryController.h"
#include "RegisterBank.h"
#include "ArithmeticLogicUnit.h"
#include "Opcodes.h"

using namespace std;
using namespace gbxcore;
using namespace gbxcore::interfaces;
using namespace gbxcore::instructions;

TEST(CoreTests_TestJR, DecodeUnconditionalJrImmediateMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto opcode = 0x18;
    alu.DecodeInstruction(opcode, nullopt);

    EXPECT_EQ(OpcodeType::jr, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::Immediate, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::PC, alu.GetInstructionData().DestinationRegister);
    EXPECT_EQ(0xFF, alu.GetInstructionData().InstructionExtraOperand);
}

TEST(CoreTests_TestJR, ExecuteUnconditionalJrImmediateMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint16_t> distribution{0x0000, 0xFFFF};
    uniform_int_distribution<uint8_t> offset{0x00, 0xFF};

    for (auto i = 0; i < 0xFFF; i++)
    {
        auto opcode = 0x18;
        alu.DecodeInstruction(opcode, nullopt);

        auto targetPCAddress = distribution(engine);
        auto targetOffset = static_cast<int8_t>(offset(engine));
        auto result = static_cast<uint16_t>(targetPCAddress + (targetOffset));

        alu.GetInstructionData().MemoryOperand1 = targetOffset;
        registerBank.WritePair(Register::PC, targetPCAddress);

        alu.Execute();

        EXPECT_EQ(result, registerBank.ReadPair(Register::PC));
    }
}

TEST(CoreTests_TestJR, DecodeConditionalJrImmediateMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    for (auto condition = 0; condition < 4; condition++)
    {
        auto opcode = 0x20 | (condition << 0x03);
        alu.DecodeInstruction(opcode, nullopt);

        EXPECT_EQ(OpcodeType::jr, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::Immediate, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(Register::PC, alu.GetInstructionData().DestinationRegister);
        EXPECT_EQ(condition, alu.GetInstructionData().InstructionExtraOperand);
    }
}

TEST(CoreTests_TestJR, ExecuteConditionalJrImmediateMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint16_t> distribution{0x0000, 0xFFFF};
    uniform_int_distribution<uint8_t> offset{0x00, 0xFF};

    for (auto i = 0; i < 0xFFF; i++)
    {
        registerBank.Write(Register::F, 0x00);

        auto initialPCAddress = distribution(engine);
        registerBank.WritePair(Register::PC, initialPCAddress);

        auto targetCondition = distribution(engine)%4;
        auto opcode =  0x20 | (targetCondition << 0x03);
        alu.DecodeInstruction(opcode, nullopt);

        auto targetOffset = static_cast<int8_t>(offset(engine));
        auto targetPCAddress = static_cast<uint16_t>(initialPCAddress + (targetOffset));

        alu.GetInstructionData().MemoryOperand1 = targetOffset;

        if (targetCondition == 0 || targetCondition == 1)
        {
            auto randomZeroValue = distribution(engine)%2;
            registerBank.WriteFlag(Flag::Z, randomZeroValue);

            alu.Execute();

            if ((targetCondition == 0 && randomZeroValue == 0) ||
                (targetCondition == 1 && randomZeroValue == 1))
                EXPECT_EQ(targetPCAddress, registerBank.ReadPair(Register::PC));
            else
                EXPECT_EQ(initialPCAddress, registerBank.ReadPair(Register::PC));
        }
        else
        {
            auto randomCarryValue = distribution(engine)%2;
            registerBank.WriteFlag(Flag::CY, randomCarryValue);

            alu.Execute();

            if ((targetCondition == 2 && randomCarryValue == 0) ||
                (targetCondition == 3 && randomCarryValue == 1))
                EXPECT_EQ(targetPCAddress, registerBank.ReadPair(Register::PC));
            else
                EXPECT_EQ(initialPCAddress, registerBank.ReadPair(Register::PC));
        }
    }
}
