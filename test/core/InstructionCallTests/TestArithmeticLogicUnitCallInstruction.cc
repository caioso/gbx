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

TEST(TestCall, DecodeUnconditionalCall)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto opcode = 0xCD;
    alu.DecodeInstruction(opcode, nullopt);

    EXPECT_EQ(OpcodeType::call, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::SubRoutineCall, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::PC, alu.GetInstructionData().DestinationRegister);
    EXPECT_EQ(0xFF, alu.GetInstructionData().InstructionExtraOperand);
}

TEST(TestCall, ExecuteUnconditionalCall)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto opcode = 0xCD;
    alu.DecodeInstruction(opcode, nullopt);

    registerBank->WritePair(Register::PC, 0x9800);
    alu.GetInstructionData().MemoryOperand1 = 0xF1;
    alu.GetInstructionData().MemoryOperand2 = 0x44;
    
    alu.Execute();

    EXPECT_EQ(registerBank->ReadPair(Register::PC), static_cast<uint16_t>(0x44F1));
    EXPECT_EQ(alu.GetInstructionData().MemoryResult1, static_cast<uint8_t>(0x98));
    EXPECT_EQ(alu.GetInstructionData().MemoryResult2, static_cast<uint8_t>(0x00));
}

TEST(TestCall, DecodeConditionalCall)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    for (auto condition = 0; condition < 4; condition++)
    {
        auto opcode = 0xC0 | condition << 0x03 | 0x04;
        alu.DecodeInstruction(opcode, nullopt);

        EXPECT_EQ(OpcodeType::call, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::SubRoutineCall, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(Register::PC, alu.GetInstructionData().DestinationRegister);
        EXPECT_EQ(condition, alu.GetInstructionData().InstructionExtraOperand);
    }
}

TEST(TestCall, ExecuteConditionalCall)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    // NZ
    auto opcode = 0xC0 | 0x00 | 0x04;
    alu.DecodeInstruction(opcode, nullopt);

    registerBank->WritePair(Register::PC, 0x9800);
    alu.GetInstructionData().MemoryOperand1 = 0xF1;
    alu.GetInstructionData().MemoryOperand2 = 0x44;
    registerBank->WriteFlag(Flag::Z, 0x00);
    
    alu.Execute();

    EXPECT_EQ(registerBank->ReadPair(Register::PC), static_cast<uint16_t>(0x44F1));
    EXPECT_EQ(alu.GetInstructionData().MemoryResult1, static_cast<uint8_t>(0x98));
    EXPECT_EQ(alu.GetInstructionData().MemoryResult2, static_cast<uint8_t>(0x00));

    alu.DecodeInstruction(opcode, nullopt);

    registerBank->WritePair(Register::PC, 0x9800);
    alu.GetInstructionData().MemoryOperand1 = 0xF1;
    alu.GetInstructionData().MemoryOperand2 = 0x44;
    registerBank->WriteFlag(Flag::Z, 0x01);
    
    alu.Execute();

    EXPECT_EQ(registerBank->ReadPair(Register::PC), static_cast<uint16_t>(0x9800));
    EXPECT_EQ(alu.GetInstructionData().MemoryResult1, static_cast<uint8_t>(0x00));
    EXPECT_EQ(alu.GetInstructionData().MemoryResult2, static_cast<uint8_t>(0x00));
    
    // Z
    opcode = 0xC0 | (0x01 << 0x03) | 0x04;
    alu.DecodeInstruction(opcode, nullopt);

    registerBank->WritePair(Register::PC, 0x9800);
    alu.GetInstructionData().MemoryOperand1 = 0xF1;
    alu.GetInstructionData().MemoryOperand2 = 0x44;
    registerBank->WriteFlag(Flag::Z, 0x01);
    
    alu.Execute();

    EXPECT_EQ(registerBank->ReadPair(Register::PC), static_cast<uint16_t>(0x44F1));
    EXPECT_EQ(alu.GetInstructionData().MemoryResult1, static_cast<uint8_t>(0x98));
    EXPECT_EQ(alu.GetInstructionData().MemoryResult2, static_cast<uint8_t>(0x00));

    alu.DecodeInstruction(opcode, nullopt);

    registerBank->WritePair(Register::PC, 0x9800);
    alu.GetInstructionData().MemoryOperand1 = 0xF1;
    alu.GetInstructionData().MemoryOperand2 = 0x44;
    registerBank->WriteFlag(Flag::Z, 0x00);
    
    alu.Execute();

    EXPECT_EQ(registerBank->ReadPair(Register::PC), static_cast<uint16_t>(0x9800));
    EXPECT_EQ(alu.GetInstructionData().MemoryResult1, static_cast<uint8_t>(0x00));
    EXPECT_EQ(alu.GetInstructionData().MemoryResult2, static_cast<uint8_t>(0x00));

    // NC
    opcode = 0xC0 | (0x02 << 3)| 0x04;
    alu.DecodeInstruction(opcode, nullopt);

    registerBank->WritePair(Register::PC, 0x9800);
    alu.GetInstructionData().MemoryOperand1 = 0xF1;
    alu.GetInstructionData().MemoryOperand2 = 0x44;
    registerBank->WriteFlag(Flag::CY, 0x00);
    
    alu.Execute();

    EXPECT_EQ(registerBank->ReadPair(Register::PC), static_cast<uint16_t>(0x44F1));
    EXPECT_EQ(alu.GetInstructionData().MemoryResult1, static_cast<uint8_t>(0x98));
    EXPECT_EQ(alu.GetInstructionData().MemoryResult2, static_cast<uint8_t>(0x00));

    alu.DecodeInstruction(opcode, nullopt);

    registerBank->WritePair(Register::PC, 0x9800);
    alu.GetInstructionData().MemoryOperand1 = 0xF1;
    alu.GetInstructionData().MemoryOperand2 = 0x44;
    registerBank->WriteFlag(Flag::CY, 0x01);
    
    alu.Execute();

    EXPECT_EQ(registerBank->ReadPair(Register::PC), static_cast<uint16_t>(0x9800));
    EXPECT_EQ(alu.GetInstructionData().MemoryResult1, static_cast<uint8_t>(0x00));
    EXPECT_EQ(alu.GetInstructionData().MemoryResult2, static_cast<uint8_t>(0x00));
    
    // C
    opcode = 0xC0 | (0x03 << 0x03) | 0x04;
    alu.DecodeInstruction(opcode, nullopt);

    registerBank->WritePair(Register::PC, 0x9800);
    alu.GetInstructionData().MemoryOperand1 = 0xF1;
    alu.GetInstructionData().MemoryOperand2 = 0x44;
    registerBank->WriteFlag(Flag::CY, 0x01);
    
    alu.Execute();

    EXPECT_EQ(registerBank->ReadPair(Register::PC), static_cast<uint16_t>(0x44F1));
    EXPECT_EQ(alu.GetInstructionData().MemoryResult1, static_cast<uint8_t>(0x98));
    EXPECT_EQ(alu.GetInstructionData().MemoryResult2, static_cast<uint8_t>(0x00));

    alu.DecodeInstruction(opcode, nullopt);

    registerBank->WritePair(Register::PC, 0x9800);
    alu.GetInstructionData().MemoryOperand1 = 0xF1;
    alu.GetInstructionData().MemoryOperand2 = 0x44;
    registerBank->WriteFlag(Flag::CY, 0x00);
    
    alu.Execute();

    EXPECT_EQ(registerBank->ReadPair(Register::PC), static_cast<uint16_t>(0x9800));
    EXPECT_EQ(alu.GetInstructionData().MemoryResult1, static_cast<uint8_t>(0x00));
    EXPECT_EQ(alu.GetInstructionData().MemoryResult2, static_cast<uint8_t>(0x00));
}