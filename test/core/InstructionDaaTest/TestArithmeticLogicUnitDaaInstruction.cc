
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

TEST(TestDaa, DecodeDaa)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto opcode = 0x27;
    alu.DecodeInstruction(opcode, nullopt);

    EXPECT_EQ(OpcodeType::daa, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::A, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().DestinationRegister);
    EXPECT_EQ(0x00, alu.GetInstructionData().InstructionExtraOperand);
}

TEST(TestDaa, ExecuteDaa)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    // Test Flags
    auto rawBinary = 0x27;

    alu.DecodeInstruction(rawBinary, nullopt);

    registerBank->WriteFlag(Flag::N, 0x00);

    // CY = 0 && H = 0, HighNibble 0-9, LowNibble 0-9
    for (auto highNibble = 0x00; highNibble <= 0x09; ++highNibble)
    {
        for (auto lowNibble = 0x00; lowNibble <= 0x09; ++lowNibble)
        {
            registerBank->WriteFlag(Flag::CY, 0x00);
            registerBank->WriteFlag(Flag::H, 0x00);

            auto aValue = static_cast<uint8_t>((highNibble << 0x04) | lowNibble);
            auto resultingAValue = static_cast<uint8_t>((highNibble << 0x04) | lowNibble);

            registerBank->Write(Register::A, aValue);

            alu.Execute();

            EXPECT_EQ(resultingAValue, registerBank->Read(Register::A));
            EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
            EXPECT_EQ((resultingAValue == 0? 0x01 : 0x00), registerBank->ReadFlag(Flag::Z));
        }
    }
    
    // CY = 0 && H = 0, HighNibble 0-8, LowNibble A-F
    for (auto highNibble = 0x00; highNibble <= 0x08; ++highNibble)
    {
        for (auto lowNibble = 0x0A; lowNibble <= 0x0F; ++lowNibble)
        {
            registerBank->WriteFlag(Flag::CY, 0x00);
            registerBank->WriteFlag(Flag::H, 0x00);

            auto aValue = static_cast<uint8_t>((highNibble << 0x04) | lowNibble);
            auto resultingAValue = static_cast<uint8_t>(((highNibble << 0x04) | lowNibble) + 0x06);

            registerBank->Write(Register::A, aValue);

            alu.Execute();

            EXPECT_EQ(resultingAValue, registerBank->Read(Register::A));
            EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
            EXPECT_EQ((resultingAValue == 0? 0x01 : 0x00), registerBank->ReadFlag(Flag::Z));
        }
    }
    
    // CY = 0 && H = 1, HighNibble 0-9, LowNibble 0-3
    for (auto highNibble = 0x00; highNibble <= 0x09; ++highNibble)
    {
        for (auto lowNibble = 0x00; lowNibble <= 0x03; ++lowNibble)
        {
            registerBank->WriteFlag(Flag::CY, 0x00);
            registerBank->WriteFlag(Flag::H, 0x01);

            auto aValue = static_cast<uint8_t>((highNibble << 0x04) | lowNibble);
            auto resultingAValue = static_cast<uint8_t>(((highNibble << 0x04) | lowNibble) + 0x06);

            registerBank->Write(Register::A, aValue);

            alu.Execute();

            EXPECT_EQ(resultingAValue, registerBank->Read(Register::A));
            EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
            EXPECT_EQ((resultingAValue == 0? 0x01 : 0x00), registerBank->ReadFlag(Flag::Z));
        }
    }
    
    // CY = 0 && H = 0, HighNibble A-F, LowNibble 0-9
    for (auto highNibble = 0x0A; highNibble <= 0x0F; ++highNibble)
    {
        for (auto lowNibble = 0x00; lowNibble <= 0x09; ++lowNibble)
        {
            registerBank->WriteFlag(Flag::CY, 0x00);
            registerBank->WriteFlag(Flag::H, 0x00);

            auto aValue = static_cast<uint8_t>((highNibble << 0x04) | lowNibble);
            auto resultingAValue = static_cast<uint8_t>(((highNibble << 0x04) | lowNibble) + 0x60);

            registerBank->Write(Register::A, aValue);

            alu.Execute();

            EXPECT_EQ(resultingAValue, registerBank->Read(Register::A));
            EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
            EXPECT_EQ((resultingAValue == 0? 0x01 : 0x00), registerBank->ReadFlag(Flag::Z));
        }
    }
    
    // CY = 0 && H = 0, HighNibble A-F, LowNibble 0-9
    for (auto highNibble = 0x09; highNibble <= 0x0F; ++highNibble)
    {
        for (auto lowNibble = 0x0A; lowNibble <= 0x0F; ++lowNibble)
        {
            registerBank->WriteFlag(Flag::CY, 0x00);
            registerBank->WriteFlag(Flag::H, 0x00);

            auto aValue = static_cast<uint8_t>((highNibble << 0x04) | lowNibble);
            auto resultingAValue = static_cast<uint8_t>(((highNibble << 0x04) | lowNibble) + 0x66);

            registerBank->Write(Register::A, aValue);

            alu.Execute();

            EXPECT_EQ(resultingAValue, registerBank->Read(Register::A));
            EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
            EXPECT_EQ((resultingAValue == 0? 0x01 : 0x00), registerBank->ReadFlag(Flag::Z));
        }
    }
    
    // CY = 0 && H = 1, HighNibble A-F, LowNibble 0-3
    for (auto highNibble = 0x0A; highNibble <= 0x0F; ++highNibble)
    {
        for (auto lowNibble = 0x00; lowNibble <= 0x03; ++lowNibble)
        {
            registerBank->WriteFlag(Flag::CY, 0x00);
            registerBank->WriteFlag(Flag::H, 0x01);

            auto aValue = static_cast<uint8_t>((highNibble << 0x04) | lowNibble);
            auto resultingAValue = static_cast<uint8_t>(((highNibble << 0x04) | lowNibble) + 0x66);

            registerBank->Write(Register::A, aValue);

            alu.Execute();

            EXPECT_EQ(resultingAValue, registerBank->Read(Register::A));
            EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
            EXPECT_EQ((resultingAValue == 0? 0x01 : 0x00), registerBank->ReadFlag(Flag::Z));
        }
    }
    
    // CY = 1 && H = 0, HighNibble 0-2, LowNibble 0-9
    for (auto highNibble = 0x00; highNibble <= 0x02; ++highNibble)
    {
        for (auto lowNibble = 0x00; lowNibble <= 0x09; ++lowNibble)
        {
            registerBank->WriteFlag(Flag::CY, 0x01);
            registerBank->WriteFlag(Flag::H, 0x00);

            auto aValue = static_cast<uint8_t>((highNibble << 0x04) | lowNibble);
            auto resultingAValue = static_cast<uint8_t>(((highNibble << 0x04) | lowNibble) + 0x60);

            registerBank->Write(Register::A, aValue);

            alu.Execute();

            EXPECT_EQ(resultingAValue, registerBank->Read(Register::A));
            EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
            EXPECT_EQ((resultingAValue == 0? 0x01 : 0x00), registerBank->ReadFlag(Flag::Z));
        }
    }
    
    // CY = 1 && H = 0, HighNibble 0-2, LowNibble A-F
    for (auto highNibble = 0x00; highNibble <= 0x02; ++highNibble)
    {
        for (auto lowNibble = 0x0A; lowNibble <= 0x0F; ++lowNibble)
        {
            registerBank->WriteFlag(Flag::CY, 0x01);
            registerBank->WriteFlag(Flag::H, 0x00);

            auto aValue = static_cast<uint8_t>((highNibble << 0x04) | lowNibble);
            auto resultingAValue = static_cast<uint8_t>(((highNibble << 0x04) | lowNibble) + 0x66);

            registerBank->Write(Register::A, aValue);

            alu.Execute();

            EXPECT_EQ(resultingAValue, registerBank->Read(Register::A));
            EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
            EXPECT_EQ((resultingAValue == 0? 0x01 : 0x00), registerBank->ReadFlag(Flag::Z));
        }
    }
    
    // CY = 1 && H = 1, HighNibble 0-3, LowNibble 0-3
    for (auto highNibble = 0x00; highNibble <= 0x03; ++highNibble)
    {
        for (auto lowNibble = 0x00; lowNibble <= 0x03; ++lowNibble)
        {
            registerBank->WriteFlag(Flag::CY, 0x01);
            registerBank->WriteFlag(Flag::H, 0x01);

            auto aValue = static_cast<uint8_t>((highNibble << 0x04) | lowNibble);
            auto resultingAValue = static_cast<uint8_t>(((highNibble << 0x04) | lowNibble) + 0x66);

            registerBank->Write(Register::A, aValue);

            alu.Execute();

            EXPECT_EQ(resultingAValue, registerBank->Read(Register::A));
            EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
            EXPECT_EQ((resultingAValue == 0? 0x01 : 0x00), registerBank->ReadFlag(Flag::Z));
        }
    }

    registerBank->WriteFlag(Flag::N, 0x01);

    // CY = 0 && H = 0, HighNibble 0-9, LowNibble 0-9
    for (auto highNibble = 0x00; highNibble <= 0x09; ++highNibble)
    {
        for (auto lowNibble = 0x00; lowNibble <= 0x09; ++lowNibble)
        {
            registerBank->WriteFlag(Flag::CY, 0x00);
            registerBank->WriteFlag(Flag::H, 0x00);

            auto aValue = static_cast<uint8_t>((highNibble << 0x04) | lowNibble);
            auto resultingAValue = static_cast<uint8_t>(((highNibble << 0x04) | lowNibble) + 0x00);

            registerBank->Write(Register::A, aValue);

            alu.Execute();

            EXPECT_EQ(resultingAValue, registerBank->Read(Register::A));
            EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
            EXPECT_EQ((resultingAValue == 0? 0x01 : 0x00), registerBank->ReadFlag(Flag::Z));
        }
    }
    
    // CY = 0 && H = 1, HighNibble 0-8, LowNibble 6-F
    for (auto highNibble = 0x00; highNibble <= 0x08; ++highNibble)
    {
        for (auto lowNibble = 0x06; lowNibble <= 0x0F; ++lowNibble)
        {
            registerBank->WriteFlag(Flag::CY, 0x00);
            registerBank->WriteFlag(Flag::H, 0x01);

            auto aValue = static_cast<uint8_t>((highNibble << 0x04) | lowNibble);
            auto resultingAValue = static_cast<uint8_t>(((highNibble << 0x04) | lowNibble) + 0xFA);

            registerBank->Write(Register::A, aValue);

            alu.Execute();

            EXPECT_EQ(resultingAValue, registerBank->Read(Register::A));
            EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
            EXPECT_EQ((resultingAValue == 0? 0x01 : 0x00), registerBank->ReadFlag(Flag::Z));
        }
    }
    
    // CY = 1 && H = 0, HighNibble 7-F, LowNibble 0-9
    for (auto highNibble = 0x07; highNibble <= 0x0F; ++highNibble)
    {
        for (auto lowNibble = 0x00; lowNibble <= 0x09; ++lowNibble)
        {
            registerBank->WriteFlag(Flag::CY, 0x01);
            registerBank->WriteFlag(Flag::H, 0x00);

            auto aValue = static_cast<uint8_t>((highNibble << 0x04) | lowNibble);
            auto resultingAValue = static_cast<uint8_t>(((highNibble << 0x04) | lowNibble) + 0xA0);

            registerBank->Write(Register::A, aValue);

            alu.Execute();

            EXPECT_EQ(resultingAValue, registerBank->Read(Register::A));
            EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
            EXPECT_EQ((resultingAValue == 0? 0x01 : 0x00), registerBank->ReadFlag(Flag::Z));
        }
    }
    
    // CY = 1 && H = 1, HighNibble 6-F, LowNibble 6-F
    for (auto highNibble = 0x06; highNibble <= 0x0F; ++highNibble)
    {
        for (auto lowNibble = 0x06; lowNibble <= 0x0F; ++lowNibble)
        {
            registerBank->WriteFlag(Flag::CY, 0x01);
            registerBank->WriteFlag(Flag::H, 0x01);

            auto aValue = static_cast<uint8_t>((highNibble << 0x04) | lowNibble);
            auto resultingAValue = static_cast<uint8_t>(((highNibble << 0x04) | lowNibble) + 0x9A);

            registerBank->Write(Register::A, aValue);

            alu.Execute();

            EXPECT_EQ(resultingAValue, registerBank->Read(Register::A));
            EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
            EXPECT_EQ((resultingAValue == 0? 0x01 : 0x00), registerBank->ReadFlag(Flag::Z));
        }
    }
}
