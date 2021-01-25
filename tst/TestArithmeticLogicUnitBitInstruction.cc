#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <optional>
#include <random>
#include <vector>

#include <iostream>

#include "../src/MemoryController.h"
#include "../src/RegisterBank.h"
#include "../src/ArithmeticLogicUnit.h"
#include "../src/instructions/Opcodes.h"

using namespace std;
using namespace gbx;
using namespace gbx::interfaces;

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

TEST(TestBit, DecodeBitRegisterMode)
{
    auto operandList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto bitCounter = static_cast<uint8_t>(0);
    for (auto operand : operandList)
    {
        auto preOpcode = 0xCB;
        auto opcode = 0x40 | ((bitCounter++) << 0x03) | RegisterBankInterface::ToInstructionSource(operand);
        alu.DecodeInstruction(opcode, preOpcode);

        EXPECT_EQ(OpcodeType::bit, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(bitCounter - 1, alu.GetInstructionData().InstructionExtraOperand);
        EXPECT_EQ(operand, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(operand, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(TestBit, ExecuteBitRegisterMode)
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
    uniform_int_distribution<uint8_t> bitDistribution{0, 7};

    for (auto i = 0; i < 1000; i++)
    {
        auto operandValue = distribution(engine);
        auto operand = *(begin(operandList) + operandDistribution(engine));
        auto targetBit = bitDistribution(engine);
        auto randomCYValue = static_cast<uint8_t>(distribution(engine) %2 == 0? 0 : 1);
        auto result = static_cast<uint8_t>((operandValue >> targetBit) & 0x01);

        auto preOpcode = 0xCB;
        auto opcode = 0x40 | (targetBit << 3) | RegisterBankInterface::ToInstructionSource(operand);
        
        alu.DecodeInstruction(opcode, preOpcode);
        registerBank->Write(operand, operandValue);
        registerBank->WriteFlag(Flag::CY, randomCYValue);

        alu.Execute();

        // Register value is not changed
        EXPECT_EQ(operandValue, registerBank->Read(operand));

        // Test Flags
        EXPECT_EQ((result == 0? 0x01 : 0x00), registerBank->ReadFlag(Flag::Z));
        EXPECT_EQ(randomCYValue, registerBank->ReadFlag(Flag::CY));
        EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    }
}
TEST(TestBit, DecodeBitRegisterIndirectMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    for (auto i = 0; i < 8; i++)
    {
        auto preOpcode = 0xCB;
        auto opcode = 0x40 | ((i) << 0x03) | 0x06;
        alu.DecodeInstruction(opcode, preOpcode);

        EXPECT_EQ(OpcodeType::bit, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::RegisterIndirectSource, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(i, alu.GetInstructionData().InstructionExtraOperand);
        EXPECT_EQ(Register::HL, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(Register::HL, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(TestBit, ExecuteBitRegisterIndirectMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> distribution{0, 255};
    uniform_int_distribution<uint8_t> bitDistribution{0, 7};

    for (auto i = 0; i < 1000; i++)
    {
        auto operandValue = distribution(engine);
        auto targetBit = bitDistribution(engine);
        auto randomCYValue = static_cast<uint8_t>(distribution(engine) %2 == 0? 0 : 1);
        auto result = static_cast<uint8_t>((operandValue >> targetBit) & 0x01);

        auto preOpcode = 0xCB;
        auto opcode = 0x40 | (targetBit << 3) | 0x06;
        
        alu.DecodeInstruction(opcode, preOpcode);
        alu.GetInstructionData().MemoryOperand1 = operandValue;
        registerBank->WriteFlag(Flag::CY, randomCYValue);

        alu.Execute();

        // Test Flags
        EXPECT_EQ((result == 0? 0x01 : 0x00), registerBank->ReadFlag(Flag::Z));
        EXPECT_EQ(randomCYValue, registerBank->ReadFlag(Flag::CY));
        EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    }
}