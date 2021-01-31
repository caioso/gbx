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

TEST(TestJp, DecodeUnconditionalJpImmediateMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto opcode = 0xC3;
    alu.DecodeInstruction(opcode, nullopt);

    EXPECT_EQ(OpcodeType::jp, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::ImmediatePair, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::PC, alu.GetInstructionData().DestinationRegister);
    EXPECT_EQ(0xFF, alu.GetInstructionData().InstructionExtraOperand);
}

TEST(TestJp, ExecuteUnconditionalJpImmediateMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint16_t> distribution{0x0000, 0xFFFF};

    for (auto i = 0; i < 0xFFF; i++)
    {
        auto opcode = 0xC3;
        alu.DecodeInstruction(opcode, nullopt);

        auto targetPCAddress = distribution(engine);
        alu.GetInstructionData().MemoryOperand1 = targetPCAddress & 0xFF;
        alu.GetInstructionData().MemoryOperand2 = (targetPCAddress >> 0x08) & 0xFF;

        alu.Execute();

        EXPECT_EQ(targetPCAddress, registerBank->ReadPair(Register::PC));
    }
}

TEST(TestJp, DecodeConditionalJpImmediateMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    for (auto condition = 0; condition < 4; condition++)
    {
        auto opcode = 0xC0 | (condition << 0x03) | 0x02;
        alu.DecodeInstruction(opcode, nullopt);

        EXPECT_EQ(OpcodeType::jp, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::ImmediatePair, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(Register::PC, alu.GetInstructionData().DestinationRegister);
        EXPECT_EQ(condition, alu.GetInstructionData().InstructionExtraOperand);
    }
}

TEST(TestJp, ExecuteConditionalJpImmediateMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint16_t> distribution{0x0000, 0xFFFF};

    for (auto i = 0; i < 0xFFF; i++)
    {
        registerBank->Write(Register::F, 0x00);

        auto initialPCAddress = distribution(engine);
        registerBank->WritePair(Register::PC, initialPCAddress);

        auto targetCondition = distribution(engine)%4;
        auto opcode = 0xC0 | (targetCondition << 0x03) | 0x02;
        alu.DecodeInstruction(opcode, nullopt);

        auto targetPCAddress = distribution(engine);
        alu.GetInstructionData().MemoryOperand1 = targetPCAddress & 0xFF;
        alu.GetInstructionData().MemoryOperand2 = (targetPCAddress >> 0x08) & 0xFF;

        if (targetCondition == 0 || targetCondition == 1)
        {
            auto randomZeroValue = distribution(engine)%2;
            registerBank->WriteFlag(Flag::Z, randomZeroValue);

            alu.Execute();

            if ((targetCondition == 0 && randomZeroValue == 0) ||
                (targetCondition == 1 && randomZeroValue == 1))
                EXPECT_EQ(targetPCAddress, registerBank->ReadPair(Register::PC));
            else
                EXPECT_EQ(initialPCAddress, registerBank->ReadPair(Register::PC));
        }
        else
        {
            auto randomCarryValue = distribution(engine)%2;
            registerBank->WriteFlag(Flag::CY, randomCarryValue);

            alu.Execute();

            if ((targetCondition == 2 && randomCarryValue == 0) ||
                (targetCondition == 3 && randomCarryValue == 1))
                EXPECT_EQ(targetPCAddress, registerBank->ReadPair(Register::PC));
            else
                EXPECT_EQ(initialPCAddress, registerBank->ReadPair(Register::PC));
        }
    }
}

TEST(TestJp, DecodeUnconditionalJpRegisterIndirectMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();
    
    auto opcode = 0xE9;
    alu.DecodeInstruction(opcode, nullopt);

    EXPECT_EQ(OpcodeType::jp, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSourcePair, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::PC, alu.GetInstructionData().DestinationRegister);
    EXPECT_EQ(0xFF, alu.GetInstructionData().InstructionExtraOperand);
}

TEST(TestJp, ExecuteUnconditionalJpRegisterIndirectMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint16_t> distribution{0x0000, 0xFFFF};

    for (auto i = 0; i < 0xFFF; i++)
    {
        auto opcode = 0xE9;
        alu.DecodeInstruction(opcode, nullopt);

        auto targetPCAddress = distribution(engine);
        alu.GetInstructionData().MemoryOperand1 = targetPCAddress & 0xFF;
        alu.GetInstructionData().MemoryOperand2 = (targetPCAddress >> 0x08) & 0xFF;

        alu.Execute();

        EXPECT_EQ(targetPCAddress, registerBank->ReadPair(Register::PC));
    }
}