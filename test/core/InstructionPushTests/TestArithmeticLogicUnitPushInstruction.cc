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

TEST(TestPush, DecodePushRegisterMode)
{
    auto registerBank = make_shared<RegisterBank>();
    auto operandList = {Register::BC, Register::DE, Register::HL, Register::AF };
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    for (auto operand : operandList)
    {
        auto rawBinary = (0x03 << 0x06) | (RegisterBank::ToInstructionRegisterPushPair(operand)) << 0x04 | 0x05;
        alu.DecodeInstruction(rawBinary, nullopt);

        EXPECT_EQ(OpcodeType::push, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::RegisterIndirectDestinationPair, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(operand, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(Register::SP, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(TestPush, ExecutePushRegisterMode)
{
    auto operandList = {Register::BC, Register::DE, Register::HL, Register::AF };
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint16_t> distribution{0, 65535};
    uniform_int_distribution<uint8_t> sourceRegisterDistribution {0, 3};

    for (auto i = 0; i < 1000; i++)
    {
        auto operandValue = distribution(engine);
        auto operand = *(begin(operandList) + sourceRegisterDistribution(engine));
        auto rawBinary = (0x03 << 0x06) | (RegisterBank::ToInstructionRegisterPushPair(operand)) << 0x04 | 0x05;

        registerBank->WritePair(operand, operandValue);
        alu.DecodeInstruction(rawBinary, nullopt);

        alu.Execute();

        EXPECT_EQ(alu.GetInstructionData().MemoryResult1, (static_cast<uint8_t>(operandValue>>8) & 0xFF));
        EXPECT_EQ(alu.GetInstructionData().MemoryResult2, (static_cast<uint8_t>(operandValue) & 0xFF));
    }
}
