#include <gtest/gtest.h>

#include "CoreTestMocksAndWrappers.h"

#include <iostream>
#include <algorithm>
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


TEST(CoreTests_TestINC, DecodeIncRegisterMode)
{
    RegisterBank registerBank;
    auto operandList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    for (auto operand : operandList)
    {
        auto rawBinary = 0x04 | (RegisterBank::ToInstructionSource(operand)) << 0x03;
        alu.DecodeInstruction(rawBinary, nullopt);

        EXPECT_EQ(OpcodeType::inc, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(operand, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(operand, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(CoreTests_TestINC, ExecuteIncRegisterMode)
{
    auto sourceList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> distribution{0, 255};
    uniform_int_distribution<uint8_t> sourceRegisterDistribution {0, 6};

    for (auto i = 0; i < 1000; i++)
    {
        auto operandValue = distribution(engine);
        auto operand = *(begin(sourceList) + sourceRegisterDistribution(engine));
        auto rawBinary = 0x04 | (RegisterBank::ToInstructionSource(operand)) << 0x03;

        registerBank.Write(operand, operandValue);
        alu.DecodeInstruction(rawBinary, nullopt);

        // randomly choose a carry value -> Inc sould not change the value of flag CY.
        auto carryValue = distribution(engine) % 2 == 0? 0 : 1;

        if (carryValue)
            registerBank.SetFlag(Flag::CY);
        else
            registerBank.ClearFlag(Flag::CY);

        alu.Execute();

        EXPECT_EQ(static_cast<uint8_t>(static_cast<uint8_t>(operandValue) + 1), registerBank.Read(operand));
        EXPECT_EQ((operandValue == 0xFF? 1 : 0), registerBank.ReadFlag(Flag::Z));
        EXPECT_EQ(0, registerBank.ReadFlag(Flag::N));
        EXPECT_EQ(carryValue, registerBank.ReadFlag(Flag::CY));
    }
}

TEST(CoreTests_TestINC, DecodeIncRegisterIndirect)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto rawBinary = 0x34;
    alu.DecodeInstruction(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::inc, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSourceAndDestination, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().DestinationRegister);
}


TEST(CoreTests_TestINC, ExecuteIncRegisterIndirectMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> distribution{0, 255};
    uniform_int_distribution<uint8_t> sourceRegisterDistribution {0, 6};

    for (auto i = 0; i < 1000; i++)
    {
        auto operandValue = distribution(engine);
        auto rawBinary = 0x34;

        alu.DecodeInstruction(rawBinary, nullopt);
        alu.GetInstructionData().MemoryOperand1 = operandValue;

        // randomly choose a carry value -> Inc sould not change the value of flag CY.
        auto carryValue = distribution(engine) % 2 == 0? 0 : 1;

        if (carryValue)
            registerBank.SetFlag(Flag::CY);
        else
            registerBank.ClearFlag(Flag::CY);

        alu.Execute();

        EXPECT_EQ(static_cast<uint8_t>(static_cast<uint8_t>(operandValue) + 1), alu.GetInstructionData().MemoryResult1);
        EXPECT_EQ((operandValue == 0xFF? 1 : 0), registerBank.ReadFlag(Flag::Z));
        EXPECT_EQ(0, registerBank.ReadFlag(Flag::N));
        EXPECT_EQ(carryValue, registerBank.ReadFlag(Flag::CY));
    }
}

TEST(CoreTests_TestINC, DecodeIncRegisterPairMode)
{
    RegisterBank registerBank;
    auto operandList = { Register::BC, Register::DE, Register::HL, Register::SP };
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    for (auto operand : operandList)
    {
        auto rawBinary = 0x03 | (RegisterBank::ToInstructionRegisterPair(operand)) << 0x04;
        alu.DecodeInstruction(rawBinary, nullopt);

        EXPECT_EQ(OpcodeType::inc, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::RegisterPair, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(operand, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(operand, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(CoreTests_TestINC, ExecuteIncRegisterPairMode)
{
    auto sourceList = { Register::BC, Register::DE, Register::HL, Register::SP };
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint16_t> distribution{0, 65535};
    uniform_int_distribution<uint8_t> sourceRegisterDistribution {0, 3};

    for (auto i = 0; i < 1000; i++)
    {
        auto operandValue = distribution(engine);
        auto operand = *(begin(sourceList) + sourceRegisterDistribution(engine));
        auto rawBinary = 0x03 | (RegisterBank::ToInstructionRegisterPair(operand)) << 0x04;

        registerBank.WritePair(operand, operandValue);
        alu.DecodeInstruction(rawBinary, nullopt);

        // randomly choose flag values. Increment of register pairs should not change the content of the flag register.
        auto carryValue = distribution(engine) % 2 == 0? 0 : 1;
        auto zeroValue = distribution(engine) % 2 == 0? 0 : 1;
        auto halfCarryValye = distribution(engine) % 2 == 0? 0 : 1;
        auto negativeValue = distribution(engine) % 2 == 0? 0 : 1;

        if (carryValue)
            registerBank.SetFlag(Flag::CY);
        else
            registerBank.ClearFlag(Flag::CY);
        
        if (zeroValue)
            registerBank.SetFlag(Flag::Z);
        else
            registerBank.ClearFlag(Flag::Z);
        
        if (halfCarryValye)
            registerBank.SetFlag(Flag::H);
        else
            registerBank.ClearFlag(Flag::H);
        
        if (negativeValue)
            registerBank.SetFlag(Flag::N);
        else
            registerBank.ClearFlag(Flag::N);

        alu.Execute();

        EXPECT_EQ(static_cast<uint16_t>(static_cast<uint16_t>(operandValue) + 1), registerBank.ReadPair(operand));
        EXPECT_EQ(carryValue, registerBank.ReadFlag(Flag::CY));
        EXPECT_EQ(zeroValue, registerBank.ReadFlag(Flag::Z));
        EXPECT_EQ(halfCarryValye, registerBank.ReadFlag(Flag::H));
        EXPECT_EQ(negativeValue, registerBank.ReadFlag(Flag::N));
    }
}