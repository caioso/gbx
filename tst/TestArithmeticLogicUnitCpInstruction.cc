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

TEST(TestOr, DecodeCpRegisterMode)
{
    auto registerBank = make_shared<RegisterBank>();
    auto operandList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    for (auto operand : operandList)
    {
        auto rawBinary = 0xB8 | RegisterBank::ToInstructionSource(operand);
        alu.DecodeInstruction(rawBinary, nullopt);

        EXPECT_EQ(OpcodeType::cp, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(operand, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(TestSubAndSbc, ExecuteCpRegisterMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    // Test Flags
    auto rawBinary = 0xB8 | RegisterBank::ToInstructionSource(Register::D);

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->Write(Register::A, 0x01);
    registerBank->Write(Register::D, 0x01);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x01, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->Write(Register::A, 0x10);
    registerBank->Write(Register::D, 0x01);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x10, registerBank->Read(Register::A));
    
    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->Write(Register::A, 0x01);
    registerBank->Write(Register::D, 0x02);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x01, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->Write(Register::A, 0x01);
    registerBank->Write(Register::D, 0xFF);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x01, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->Write(Register::A, 0x10);
    registerBank->Write(Register::D, 0x20);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x10, registerBank->Read(Register::A));
}