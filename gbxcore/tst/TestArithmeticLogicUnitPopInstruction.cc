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

TEST(TestPop, DecodePopRegisterMode)
{
    auto registerBank = make_shared<RegisterBank>();
    auto operandList = {Register::BC, Register::DE, Register::HL, Register::AF };
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    for (auto operand : operandList)
    {
        auto rawBinary = (0x03 << 0x06) | (RegisterBank::ToInstructionRegisterPushPair(operand)) << 0x04 | 0x01;
        alu.DecodeInstruction(rawBinary, nullopt);

        EXPECT_EQ(OpcodeType::pop, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::RegisterIndirectSourcePair, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(Register::SP, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(operand, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(TestPop, ExecutePopRegisterMode)
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
        auto rawBinary = (0x03 << 0x06) | (RegisterBank::ToInstructionRegisterPushPair(operand)) << 0x04 | 0x01;

        alu.DecodeInstruction(rawBinary, nullopt);
        
        alu.GetInstructionData().MemoryOperand1 = static_cast<uint8_t>((operandValue) & 0xFF);
        alu.GetInstructionData().MemoryOperand2 = static_cast<uint8_t>((operandValue >> 0x08) & 0xFF);

        alu.Execute();

        EXPECT_EQ(operandValue, registerBank->ReadPair(operand));
    }
}