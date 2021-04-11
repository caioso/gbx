#include <gtest/gtest.h>

#include "CoreTestMocksAndWrappers.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <random>
#include <vector>

#include "ArithmeticLogicUnit.h"
#include "MemoryController.h"
#include "Opcodes.h"
#include "RegisterBank.h"

using namespace std;
using namespace gbxcore;
using namespace gbxcore::interfaces;
using namespace gbxcore::instructions;

TEST(TestRetAndReti, DecodeRet)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto opcode = 0xC9;
    alu.DecodeInstruction(opcode, nullopt);

    EXPECT_EQ(OpcodeType::ret, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSourcePair, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::SP, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::PC, alu.GetInstructionData().DestinationRegister);
    EXPECT_EQ(0xFF, alu.GetInstructionData().InstructionExtraOperand);
}

TEST(TestRetAndReti, ExecuteRet)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto opcode = 0xC9;
    alu.DecodeInstruction(opcode, nullopt);

    registerBank.WritePair(Register::PC, 0x9800);
    alu.GetInstructionData().MemoryOperand1 = 0xCD;
    alu.GetInstructionData().MemoryOperand2 = 0xAB;
    
    alu.Execute();

    EXPECT_EQ(registerBank.ReadPair(Register::PC), static_cast<uint16_t>(0xABCD));
}

TEST(TestRetAndReti, DecodeConditionalRet)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

   for (auto condition = 0; condition < 4; condition++)
    {
        auto opcode = 0xC0 | condition << 0x03 | 0x00;
        alu.DecodeInstruction(opcode, nullopt);

        EXPECT_EQ(OpcodeType::ret, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::RegisterIndirectSourcePair, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(Register::SP, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(Register::PC, alu.GetInstructionData().DestinationRegister);
        EXPECT_EQ(condition, alu.GetInstructionData().InstructionExtraOperand);
    }
}

TEST(TestRetAndReti, ExecuteConditionalRet)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    // NZ
    auto opcode = 0xC0 | 0x00 | 0x00;
    alu.DecodeInstruction(opcode, nullopt);

    registerBank.WritePair(Register::PC, 0x9800);
    alu.GetInstructionData().MemoryOperand1 = 0xF1;
    alu.GetInstructionData().MemoryOperand2 = 0x44;
    registerBank.WriteFlag(Flag::Z, 0x00);
    
    alu.Execute();

    EXPECT_EQ(registerBank.ReadPair(Register::PC), static_cast<uint16_t>(0x44F1));
    
    alu.DecodeInstruction(opcode, nullopt);

    registerBank.WritePair(Register::PC, 0x9800);
    alu.GetInstructionData().MemoryOperand1 = 0xF1;
    alu.GetInstructionData().MemoryOperand2 = 0x44;
    registerBank.WriteFlag(Flag::Z, 0x01);
    
    alu.Execute();

    EXPECT_EQ(registerBank.ReadPair(Register::PC), static_cast<uint16_t>(0x9800));
    
    // Z
    opcode = 0xC0 | (0x01 << 0x03) | 0x00;
    alu.DecodeInstruction(opcode, nullopt);

    registerBank.WritePair(Register::PC, 0x9800);
    alu.GetInstructionData().MemoryOperand1 = 0xF1;
    alu.GetInstructionData().MemoryOperand2 = 0x44;
    registerBank.WriteFlag(Flag::Z, 0x01);
    
    alu.Execute();

    EXPECT_EQ(registerBank.ReadPair(Register::PC), static_cast<uint16_t>(0x44F1));
    
    alu.DecodeInstruction(opcode, nullopt);

    registerBank.WritePair(Register::PC, 0x9800);
    alu.GetInstructionData().MemoryOperand1 = 0xF1;
    alu.GetInstructionData().MemoryOperand2 = 0x44;
    registerBank.WriteFlag(Flag::Z, 0x00);
    
    alu.Execute();

    EXPECT_EQ(registerBank.ReadPair(Register::PC), static_cast<uint16_t>(0x9800));
    
    // NC
    opcode = 0xC0 | (0x02 << 3)| 0x00;
    alu.DecodeInstruction(opcode, nullopt);

    registerBank.WritePair(Register::PC, 0x9800);
    alu.GetInstructionData().MemoryOperand1 = 0xF1;
    alu.GetInstructionData().MemoryOperand2 = 0x44;
    registerBank.WriteFlag(Flag::CY, 0x00);
    
    alu.Execute();

    EXPECT_EQ(registerBank.ReadPair(Register::PC), static_cast<uint16_t>(0x44F1));
    
    alu.DecodeInstruction(opcode, nullopt);

    registerBank.WritePair(Register::PC, 0x9800);
    alu.GetInstructionData().MemoryOperand1 = 0xF1;
    alu.GetInstructionData().MemoryOperand2 = 0x44;
    registerBank.WriteFlag(Flag::CY, 0x01);
    
    alu.Execute();

    EXPECT_EQ(registerBank.ReadPair(Register::PC), static_cast<uint16_t>(0x9800));
    
    // C
    opcode = 0xC0 | (0x03 << 0x03) | 0x00;
    alu.DecodeInstruction(opcode, nullopt);

    registerBank.WritePair(Register::PC, 0x9800);
    alu.GetInstructionData().MemoryOperand1 = 0xF1;
    alu.GetInstructionData().MemoryOperand2 = 0x44;
    registerBank.WriteFlag(Flag::CY, 0x01);
    
    alu.Execute();

    EXPECT_EQ(registerBank.ReadPair(Register::PC), static_cast<uint16_t>(0x44F1));
    
    alu.DecodeInstruction(opcode, nullopt);

    registerBank.WritePair(Register::PC, 0x9800);
    alu.GetInstructionData().MemoryOperand1 = 0xF1;
    alu.GetInstructionData().MemoryOperand2 = 0x44;
    registerBank.WriteFlag(Flag::CY, 0x00);
    
    alu.Execute();

    EXPECT_EQ(registerBank.ReadPair(Register::PC), static_cast<uint16_t>(0x9800));
}

TEST(TestRetAndReti, DecodeReti)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto opcode = 0xD9;
    alu.DecodeInstruction(opcode, nullopt);

    EXPECT_EQ(OpcodeType::reti, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSourcePair, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::SP, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::PC, alu.GetInstructionData().DestinationRegister);
    EXPECT_EQ(0xFF, alu.GetInstructionData().InstructionExtraOperand);
}

TEST(TestRetAndReti, ExecuteReti)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto opcode = 0xD9;
    alu.DecodeInstruction(opcode, nullopt);

    registerBank.WritePair(Register::PC, 0x9800);
    alu.GetInstructionData().MemoryOperand1 = 0xCD;
    alu.GetInstructionData().MemoryOperand2 = 0xAB;
    
    alu.Execute();

    EXPECT_EQ(registerBank.ReadPair(Register::PC), static_cast<uint16_t>(0xABCD));
}