#include <gtest/gtest.h>

#include "CoreTestMocksAndWrappers.h"

#include <algorithm>
#include <memory>
#include <optional>
#include <random>
#include <vector>

#include <iostream>

#include "ArithmeticLogicUnit.h"
#include "MemoryController.h"
#include "Opcodes.h"
#include "RegisterBank.h"

using namespace std;
using namespace gbxcore;
using namespace gbxcore::interfaces;
using namespace gbxcore::instructions;


TEST(CoreTests_TestHALT, DecodeHalt)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto opcode = 0x76;
    alu.DecodeInstruction(opcode, nullopt);

    EXPECT_EQ(OpcodeType::halt, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().DestinationRegister);
    EXPECT_EQ(0x00, alu.GetInstructionData().InstructionExtraOperand);
}

TEST(CoreTests_TestHALT, ExecuteHalt)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> distribution{0x00, 0xFF};

    auto opcode = 0x76;
    for (auto i = static_cast<uint8_t>(0); i < static_cast<uint8_t>(0xFF); ++i)
    {
        alu.DecodeInstruction(opcode, nullopt);
        
        auto carry = distribution(engine) % 2;
        auto zero = distribution(engine) % 2;
        auto halfCarry = distribution(engine) % 2;
        auto negative = distribution(engine) % 2;

        auto registerA = distribution(engine);
        registerBank.Write(Register::A, registerA);
        auto registerB = distribution(engine);
        registerBank.Write(Register::B, registerB);
        auto registerC = distribution(engine);
        registerBank.Write(Register::C, registerC);
        auto registerD = distribution(engine);
        registerBank.Write(Register::D, registerD);
        auto registerE = distribution(engine);
        registerBank.Write(Register::E, registerE);
        auto registerH = distribution(engine);
        registerBank.Write(Register::H, registerH);
        auto registerL = distribution(engine);
        registerBank.Write(Register::L, registerL);
        auto registerSP = static_cast<uint16_t>(distribution(engine) << 0x08 | distribution(engine));
        registerBank.WritePair(Register::SP, registerSP);
        auto registerPC = static_cast<uint16_t>(distribution(engine) << 0x08 | distribution(engine));
        registerBank.WritePair(Register::PC, registerPC);

        registerBank.WriteFlag(Flag::Z, zero);
        registerBank.WriteFlag(Flag::CY, carry);
        registerBank.WriteFlag(Flag::N, negative);
        registerBank.WriteFlag(Flag::H, halfCarry);

        alu.Execute();

        EXPECT_EQ(registerA, registerBank.Read(Register::A));
        EXPECT_EQ(registerB, registerBank.Read(Register::B));
        EXPECT_EQ(registerC, registerBank.Read(Register::C));
        EXPECT_EQ(registerD, registerBank.Read(Register::D));
        EXPECT_EQ(registerE, registerBank.Read(Register::E));
        EXPECT_EQ(registerH, registerBank.Read(Register::H));
        EXPECT_EQ(registerL, registerBank.Read(Register::L));
        EXPECT_EQ(registerSP, registerBank.ReadPair(Register::SP));
        EXPECT_EQ(registerPC, registerBank.ReadPair(Register::PC));
        
        EXPECT_EQ(carry, registerBank.ReadFlag(Flag::CY));
        EXPECT_EQ(zero, registerBank.ReadFlag(Flag::Z));
        EXPECT_EQ(halfCarry, registerBank.ReadFlag(Flag::H));
        EXPECT_EQ(negative, registerBank.ReadFlag(Flag::N));
    }
}