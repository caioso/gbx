#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <optional>
#include <random>
#include <vector>

#include <iostream>

#include "../src/instructions/Opcodes.h"
#include "../src/ArithmeticLogicUnit.h"
#include "../src/GBXExceptions.h"
#include "../src/MemoryController.h"
#include "../src/RegisterBank.h"

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

TEST(TestCcfAndScf, DecodeCcf)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto opcode = 0x3F;
    alu.DecodeInstruction(opcode, nullopt);

    EXPECT_EQ(OpcodeType::ccf, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().DestinationRegister);
    EXPECT_EQ(0x00, alu.GetInstructionData().InstructionExtraOperand);
}

TEST(TestCcfAndScf, ExecuteCcf)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto opcode = 0x3F;
    alu.DecodeInstruction(opcode, nullopt);

    registerBank->WriteFlag(Flag::CY, 1);
    registerBank->WriteFlag(Flag::H, 1);
    registerBank->WriteFlag(Flag::N, 1);
    registerBank->WriteFlag(Flag::Z, 1);

    alu.Execute();

    ASSERT_EQ(0, registerBank->ReadFlag(Flag::CY));
    ASSERT_EQ(0, registerBank->ReadFlag(Flag::H));
    ASSERT_EQ(0, registerBank->ReadFlag(Flag::N));
    ASSERT_EQ(1, registerBank->ReadFlag(Flag::Z));
    
    alu.DecodeInstruction(opcode, nullopt);

    registerBank->WriteFlag(Flag::CY, 0);
    registerBank->WriteFlag(Flag::H, 1);
    registerBank->WriteFlag(Flag::N, 1);
    registerBank->WriteFlag(Flag::Z, 0);

    alu.Execute();

    ASSERT_EQ(1, registerBank->ReadFlag(Flag::CY));
    ASSERT_EQ(0, registerBank->ReadFlag(Flag::H));
    ASSERT_EQ(0, registerBank->ReadFlag(Flag::N));
    ASSERT_EQ(0, registerBank->ReadFlag(Flag::Z));
}

TEST(TestCcfAndScf, DecodeScf)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto opcode = 0x37;
    alu.DecodeInstruction(opcode, nullopt);

    EXPECT_EQ(OpcodeType::scf, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().DestinationRegister);
    EXPECT_EQ(0x00, alu.GetInstructionData().InstructionExtraOperand);
}

TEST(TestCcfAndScf, ExecuteScf)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto opcode = 0x37;
    alu.DecodeInstruction(opcode, nullopt);

    registerBank->WriteFlag(Flag::CY, 0);
    registerBank->WriteFlag(Flag::H, 1);
    registerBank->WriteFlag(Flag::N, 1);
    registerBank->WriteFlag(Flag::Z, 1);

    alu.Execute();

    ASSERT_EQ(1, registerBank->ReadFlag(Flag::CY));
    ASSERT_EQ(0, registerBank->ReadFlag(Flag::H));
    ASSERT_EQ(0, registerBank->ReadFlag(Flag::N));
    ASSERT_EQ(1, registerBank->ReadFlag(Flag::Z));
    
    alu.DecodeInstruction(opcode, nullopt);

    registerBank->WriteFlag(Flag::CY, 1);
    registerBank->WriteFlag(Flag::H, 1);
    registerBank->WriteFlag(Flag::N, 1);
    registerBank->WriteFlag(Flag::Z, 0);

    alu.Execute();

    ASSERT_EQ(1, registerBank->ReadFlag(Flag::CY));
    ASSERT_EQ(0, registerBank->ReadFlag(Flag::H));
    ASSERT_EQ(0, registerBank->ReadFlag(Flag::N));
    ASSERT_EQ(0, registerBank->ReadFlag(Flag::Z));
}
