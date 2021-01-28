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

TEST(TestJr, DecodeUnconditionalJrImmediateMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto opcode = 0x18;
    alu.DecodeInstruction(opcode, nullopt);

    EXPECT_EQ(OpcodeType::jr, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::Immediate, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::PC, alu.GetInstructionData().DestinationRegister);
    EXPECT_EQ(0x00, alu.GetInstructionData().InstructionExtraOperand);
}

/*
TEST(TestJr, ExecuteUnconditionalJrImmediateMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint16_t> distribution{0x0000, 0xFFFF};
    uniform_int_distribution<uint8_t> offset{0x0000, 0xFF};

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
}*/