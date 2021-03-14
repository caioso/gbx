#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <optional>
#include <random>
#include <vector>

#include <iostream>

#include "MemoryController.h"
#include "RegisterBank.h"
#include "ArithmeticLogicUnit.h"
#include "instructions/Opcodes.h"

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

TEST(TestLdhl, DecodeLdhlRegisterMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto rawBinary = 0xF8;
    alu.DecodeInstruction(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::ldhl, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::Immediate, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::SP, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().DestinationRegister);
}

TEST(TestLdhl, ExecuteLdhlRegisterMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto rawBinary = 0xF8;

    alu.DecodeInstruction(rawBinary, nullopt);
    
    registerBank->WritePair(Register::SP, 0x5000);
    alu.GetInstructionData().MemoryOperand1 = 0x7F;
    
    alu.Execute();

    EXPECT_EQ(0x507F, registerBank->ReadPair(Register::HL));
    EXPECT_EQ(0, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0, registerBank->ReadFlag(Flag::Z));

    registerBank->WritePair(Register::SP, 0x5000);
    alu.GetInstructionData().MemoryOperand1 = 0xFF;
    
    alu.Execute();

    EXPECT_EQ(0x4FFF, registerBank->ReadPair(Register::HL));
    EXPECT_EQ(1, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0, registerBank->ReadFlag(Flag::Z));

    registerBank->WritePair(Register::SP, 0x5000);
    alu.GetInstructionData().MemoryOperand1 = 0xFC;
    
    alu.Execute();

    EXPECT_EQ(0x4FFC, registerBank->ReadPair(Register::HL));
    EXPECT_EQ(1, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0, registerBank->ReadFlag(Flag::Z));

    registerBank->WritePair(Register::SP, 0x4FFF);
    alu.GetInstructionData().MemoryOperand1 = 0x01;
    
    alu.Execute();

    EXPECT_EQ(0x5000, registerBank->ReadPair(Register::HL));
    EXPECT_EQ(0, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(1, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0, registerBank->ReadFlag(Flag::Z));
}