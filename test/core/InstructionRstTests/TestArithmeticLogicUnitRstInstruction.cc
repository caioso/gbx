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

uint8_t GetPageLowerAddress(uint8_t page)
{
    switch (page)
    {
        case 0x00: return 0x00;
        case 0x01: return 0x08;
        case 0x02: return 0x10;
        case 0x03: return 0x18;
        case 0x04: return 0x20;
        case 0x05: return 0x28;
        case 0x06: return 0x30;
        case 0x07: return 0x38;
    }
    return 0x00;
}

TEST(TestRst, DecodeRst)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    for (auto page = 0; page < 0x08; ++page)
    {
        auto opcode = 0xC0 | page << 0x03 | 0x07;
        alu.DecodeInstruction(opcode, nullopt);

        EXPECT_EQ(OpcodeType::rst, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::RegisterIndirectDestinationPair, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(Register::PC, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(Register::SP, alu.GetInstructionData().DestinationRegister);
        EXPECT_EQ(page, alu.GetInstructionData().InstructionExtraOperand);
    }
}

TEST(TestRst, ExecuteRst)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint16_t> distribution{0x0000, 0xFFFF};

    for (auto page = 0; page < 0x08; ++page)
    {
        auto initialPcValue =  distribution(engine);
        auto opcode = 0xC0 | page << 0x03 | 0x07;
        alu.DecodeInstruction(opcode, nullopt);

        registerBank->WritePair(Register::PC, initialPcValue);

        alu.Execute();

        EXPECT_EQ(static_cast<uint16_t>(GetPageLowerAddress(page)), registerBank->ReadPair(Register::PC));
        EXPECT_EQ(alu.GetInstructionData().MemoryResult1, static_cast<uint8_t>((initialPcValue >> 0x08) & 0xFF));
        EXPECT_EQ(alu.GetInstructionData().MemoryResult2, static_cast<uint8_t>((initialPcValue & 0xFF)));
    }
}
