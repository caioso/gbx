#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <optional>
#include <random>
#include <vector>

#include <iostream>

#include "MemoryController.h"
#include "RegisterBank.h"
#include "ArithmeticLogicUnit.h"
#include "instructions/Opcodes.h"

using namespace std;
using namespace gbxcore;
using namespace gbxcore::interfaces;
using namespace gbxcore::instructions;

class ArithmeticLogicDecorator : public ArithmeticLogicUnit
{
public:
    interfaces::DecodedInstruction& GetInstructionData() { return _instructionData; }
    void DecodeInstruction(uint8_t opcode, optional<uint8_t> preOpcode)
    {
        _registers->Write(Register::IR, opcode);
        _registers->Write(Register::PIR, preOpcode.value_or(0x00));
        Decode();
    }
};

TEST(TestAnd, DecodeAndRegisterMode)
{
    auto registerBank = make_shared<RegisterBank>();
    auto operandList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    for (auto operand : operandList)
    {
        auto rawBinary = 0xA0 | RegisterBank::ToInstructionSource(operand);
        alu.DecodeInstruction(rawBinary, nullopt);

        EXPECT_EQ(OpcodeType::bitwiseand, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(operand, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(TestAnd, ExecuteAndRegisterMode)
{
    auto registerBank = make_shared<RegisterBank>();
    auto operandList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> distribution{0, 255};
    uniform_int_distribution<uint8_t> sourceRegisterDistribution {0, 6};

    for (auto i = 0; i < 1000; i++)
    {
        auto operand = *(begin(operandList) + sourceRegisterDistribution(engine));
        auto rawBinary = 0xA0 | RegisterBank::ToInstructionSource(operand);
        auto operand1Value = distribution(engine);
        auto operand2Value = distribution(engine);
        auto result = static_cast<uint8_t>(0x00);

        alu.DecodeInstruction(rawBinary, nullopt);

        if(operand != Register::A)
        {
            result = operand1Value & operand2Value;
            registerBank->Write(Register::A, operand1Value);
            registerBank->Write(operand, operand2Value);
        }
        else
        {
            result = operand1Value & operand1Value;
            registerBank->Write(Register::A, operand1Value);
        }

        alu.Execute();

        EXPECT_EQ(result, registerBank->Read(Register::A));

        // Test Flags
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
        EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
        EXPECT_EQ(registerBank->Read(Register::A) == 0? 0x01 : 0x00, registerBank->ReadFlag(Flag::Z));
    }
}

TEST(TestAnd, DecodeAndImmediateMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto rawBinary = 0xE6;
    alu.DecodeInstruction(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::bitwiseand, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::Immediate, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
}

TEST(TestAnd, ExecuteAndImmediateMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> distribution{0, 255};
    uniform_int_distribution<uint8_t> sourceRegisterDistribution {0, 6};

    for (auto i = 0; i < 1000; i++)
    {
        auto rawBinary = 0xE6;
        auto operand1Value = distribution(engine);
        auto operand2Value = distribution(engine);
        auto result = static_cast<uint8_t>(0x00);

        alu.DecodeInstruction(rawBinary, nullopt);

        result = operand1Value & operand2Value;
        registerBank->Write(Register::A, operand1Value);
        alu.GetInstructionData().MemoryOperand1 = operand2Value;

        alu.Execute();

        EXPECT_EQ(result, registerBank->Read(Register::A));

        // Test Flags
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
        EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
        EXPECT_EQ(registerBank->Read(Register::A) == 0? 0x01 : 0x00, registerBank->ReadFlag(Flag::Z));
    }
}

TEST(TestAnd, DecodeAndRegisterIndirectMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto rawBinary = 0xA6;
    alu.DecodeInstruction(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::bitwiseand, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSource, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
}

TEST(TestAnd, ExecuteAndRegisterIndirecteMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> distribution{0, 255};
    uniform_int_distribution<uint8_t> sourceRegisterDistribution {0, 6};

    for (auto i = 0; i < 1000; i++)
    {
        auto rawBinary = 0xA6;
        auto operand1Value = distribution(engine);
        auto operand2Value = distribution(engine);
        auto result = static_cast<uint8_t>(0x00);

        alu.DecodeInstruction(rawBinary, nullopt);

        result = operand1Value & operand2Value;
        registerBank->Write(Register::A, operand1Value);
        alu.GetInstructionData().MemoryOperand1 = operand2Value;

        alu.Execute();

        EXPECT_EQ(result, registerBank->Read(Register::A));

        // Test Flags
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
        EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
        EXPECT_EQ(registerBank->Read(Register::A) == 0? 0x01 : 0x00, registerBank->ReadFlag(Flag::Z));
    }
}