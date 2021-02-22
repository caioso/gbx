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

TEST(TestSra, DecodeSraRegisterMode)
{
    auto operandList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    for (auto operand : operandList)
    {
        auto preOpcode = 0xCB;
        auto opcode = 0x28 | RegisterBankInterface::ToInstructionSource(operand);
        alu.DecodeInstruction(opcode, preOpcode);

        EXPECT_EQ(OpcodeType::sra, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(operand, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(operand, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(TestSra, ExecuteSraRegisterMode)
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
        auto mSBit = static_cast<uint8_t>((operandValue >> 7) & 0x01);
        auto lSBit = static_cast<uint8_t>((operandValue) & 0x01);
        auto result = static_cast<uint8_t>(static_cast<uint8_t>(static_cast<uint8_t>(operandValue) >> 1) | (mSBit << 7));

        auto preOpcode = 0xCB;
        auto opcode = 0x28 | RegisterBankInterface::ToInstructionSource(operand);
        alu.DecodeInstruction(opcode, preOpcode);
        registerBank->Write(operand, operandValue);

        alu.Execute();

        EXPECT_EQ(result, registerBank->Read(operand));

        // Test Flags
        EXPECT_EQ(lSBit, registerBank->ReadFlag(Flag::CY));
        EXPECT_EQ((result == 0? 0x01 : 0x00), registerBank->ReadFlag(Flag::Z));
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    }
}
TEST(TestSra, DecodeSraRegisterIndirectMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xCB;
    auto opcode = 0x2E;
    alu.DecodeInstruction(opcode, preOpcode);

    EXPECT_EQ(OpcodeType::sra, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSourceAndDestination, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().DestinationRegister);
}

TEST(TestSra, ExecuteSraRegisterIndirectMode)
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
        auto mSBit = static_cast<uint8_t>((operandValue >> 7) & 0x01);
        auto lSBit = static_cast<uint8_t>((operandValue) & 0x01);
        auto result = static_cast<uint8_t>(static_cast<uint8_t>(static_cast<uint8_t>(operandValue) >> 1) | (mSBit << 7));

        auto preOpcode = 0xCB;
        auto opcode = 0x2E;
        
        alu.DecodeInstruction(opcode, preOpcode);
        alu.GetInstructionData().MemoryOperand1 = operandValue;

        alu.Execute();

        EXPECT_EQ(result, alu.GetInstructionData().MemoryResult1);

        // Test Flags
        EXPECT_EQ(lSBit, registerBank->ReadFlag(Flag::CY));
        EXPECT_EQ((result == 0? 0x01 : 0x00), registerBank->ReadFlag(Flag::Z));
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    }
}
