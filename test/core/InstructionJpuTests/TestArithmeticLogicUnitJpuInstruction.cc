#include <gtest/gtest.h>

#include "CoreTestMocksAndWrappers.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <random>
#include <vector>

#include "MemoryController.h"
#include "RegisterBank.h"
#include "ArithmeticLogicUnit.h"
#include "Opcodes.h"

using namespace std;
using namespace gbxcore;
using namespace gbxcore::interfaces;
using namespace gbxcore::instructions;

TEST(CoreTests_TestJPU, DecodeJpu)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto preOpcode = 0xFC;
    auto opcode = 0xC3;
    alu.DecodeInstruction(opcode, preOpcode);

    EXPECT_EQ(OpcodeType::jpu, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::ImmediatePair, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::PC, alu.GetInstructionData().DestinationRegister);
    EXPECT_EQ(0xFF, alu.GetInstructionData().InstructionExtraOperand);
}

TEST(CoreTests_TestJPU, ExecuteJpu)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint16_t> distribution{0x0000, 0xFFFF};

    for (auto i = 0; i < 0xFFF; i++)
    {
        auto preOpcode = 0xFC;
        auto opcode = 0xC3;
        alu.DecodeInstruction(opcode, preOpcode);

        auto targetPCAddress = distribution(engine);
        alu.GetInstructionData().MemoryOperand1 = targetPCAddress & 0xFF;
        alu.GetInstructionData().MemoryOperand2 = (targetPCAddress >> 0x08) & 0xFF;

        alu.Execute();

        EXPECT_EQ(targetPCAddress, registerBank.ReadPair(Register::PC));
    }
}
