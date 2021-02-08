
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

TEST(TestCpl, DecodeCpl)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto opcode = 0x2F;
    alu.DecodeInstruction(opcode, nullopt);

    EXPECT_EQ(OpcodeType::cpl, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::A, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
    EXPECT_EQ(0x00, alu.GetInstructionData().InstructionExtraOperand);
}

TEST(TestCpl, ExecuteCpl)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> distribution{0x00, 0xFF};

    auto opcode = 0x2F;
    for (auto i = static_cast<uint8_t>(0); i < static_cast<uint8_t>(0xFF); ++i)
    {
        alu.DecodeInstruction(opcode, nullopt);
        
        auto carry = distribution(engine) % 2;
        auto zero = distribution(engine) % 2;

        registerBank->Write(Register::A, i);
        registerBank->WriteFlag(Flag::Z, zero);
        registerBank->WriteFlag(Flag::CY, carry);

        alu.Execute();

        EXPECT_EQ(static_cast<uint8_t>(~i), registerBank->Read(Register::A));
        EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::N));
        EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
        EXPECT_EQ(carry, registerBank->ReadFlag(Flag::CY));
        EXPECT_EQ(zero, registerBank->ReadFlag(Flag::Z));
    }
}