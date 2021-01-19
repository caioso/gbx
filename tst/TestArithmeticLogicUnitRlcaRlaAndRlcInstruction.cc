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

TEST(TestRlcaRlaAndRlc, DecodeRlcaRegisterMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto rawBinary = 0x07;
    alu.DecodeInstruction(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::rlca, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::A, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
}

TEST(TestRlcaRlaAndRlc, ExecuteRlcaRegisterMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
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
        registerBank->Write(Register::A, operandValue);

        alu.Execute();

        EXPECT_EQ(result, registerBank->Read(Register::A));

        // Test Flags
        EXPECT_EQ(static_cast<uint8_t>(operandValueMSBit), registerBank->ReadFlag(Flag::CY));
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    }
}

TEST(TestRlcaRlaAndRlc, DecodeRla)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto rawBinary = 0x17;
    alu.DecodeInstruction(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::rla, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::A, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
}

TEST(TestRlcaRlaAndRlc, ExecuteRlaRegisterMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
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
        registerBank->Write(Register::A, operandValue);
        registerBank->WriteFlag(Flag::CY, carryValue);

        alu.Execute();

        EXPECT_EQ(result, registerBank->Read(Register::A));

        // Test Flags
        EXPECT_EQ(operandValueMSBit, registerBank->ReadFlag(Flag::CY));
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    }
}

TEST(TestRlcaRlaAndRlc, DecodeRlcRegisterMode)
{
    auto registerBank = make_shared<RegisterBank>();
    auto operandList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};

    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto opcode = 0xCB;

    for (auto operand : operandList)
    {
        auto complement = RegisterBankInterface::ToInstructionSource(operand);
        cout << static_cast<size_t>(operand) << ' ' << static_cast<size_t>(complement) << '\n';
        alu.DecodeInstruction(opcode, make_optional<uint8_t>(complement));

        EXPECT_EQ(OpcodeType::rlc, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(operand, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(operand, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(TestRlcaRlaAndRlc, ExecuteRlcRegisterMode)
{
    auto operandList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> distribution{0, 255};
    uniform_int_distribution<uint8_t> zeroDistribution{0, 255};
    uniform_int_distribution<uint8_t> sourceDistribution{0, 6};

    for (auto i = 0; i < 1000; i++)
    {
        auto opcode = 0xCB;
        auto operandValue = distribution(engine);
        auto zeroValue = static_cast<uint8_t>(zeroDistribution(engine) < 128 ? 0 : 1);
        auto operand = *(begin(operandList) + sourceDistribution(engine));
        auto operandValueMSBit = static_cast<uint8_t>((operandValue >> 7) & 0x01);
        auto result = static_cast<uint8_t>((operandValue << 1) | operandValueMSBit);

        auto complement = RegisterBankInterface::ToInstructionSource(operand);
        alu.DecodeInstruction(opcode, complement);
        registerBank->Write(operand, operandValue);
        registerBank->WriteFlag(Flag::Z, zeroValue); // Zero may not be changed by the instruction
        registerBank->WriteFlag(Flag::CY, operandValueMSBit);
        alu.GetInstructionData().MemoryOperand1 = RegisterBankInterface::ToInstructionSource(operand);
        alu.Execute();

        EXPECT_EQ(result, registerBank->Read(operand));

        // Test Flags
        EXPECT_EQ(operandValueMSBit, registerBank->ReadFlag(Flag::CY));
        EXPECT_EQ(zeroValue, registerBank->ReadFlag(Flag::Z));
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
        EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    }
}
/*
TEST(TestRlcaRlaAndRlc, DecodeRlcRegisterIndirect)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto rawBinary = 0xCB;
    alu.DecodeInstruction(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::rlc, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::Immediate, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::NoRegiser, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::NoRegiser, alu.GetInstructionData().DestinationRegister);
}*/
