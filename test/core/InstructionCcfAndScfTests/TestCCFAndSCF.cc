#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <optional>
#include <random>
#include <vector>

#include <iostream>

#include "instructions/Opcodes.h"
#include "ArithmeticLogicUnit.h"
#include "GBXCoreExceptions.h"
#include "MemoryController.h"
#include "RegisterBank.h"

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

TEST(CoreTests_CCFAndSCF, DecodeCcf)
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

TEST(CoreTests_CCFAndSCF, ExecuteCcf)
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

TEST(CoreTests_CCFAndSCF, DecodeScf)
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

TEST(CoreTests_CCFAndSCF, ExecuteScf)
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
