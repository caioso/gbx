#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <optional>
#include <vector>

#include "../src/RegisterBank.h"
#include "../src/instructions/LD.h"

using namespace std;
using namespace gbx;

uint8_t BinaryRegisterAddressingMode(uint8_t opcode, Register source, Register destination)
{
    return RegisterBank::ToInstructionSource(source) | 
           RegisterBank::ToInstructionDestination(destination) << 3 |
           opcode << 6;
}

TEST(TestLD, DecodingCorrectRegisterAddressingMode)
{
    auto sourcesList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::F, Register::L};
    auto destinationsList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};

    for (auto source : sourcesList)
    for (auto destination : destinationsList)
    {
        LD ld;
        auto rawBinary = BinaryRegisterAddressingMode(0x01, source, destination);
        ld.Decode(rawBinary);

        EXPECT_NE(nullopt, ld.InstructionData);
        EXPECT_EQ(OpcodeType::ld, ld.InstructionData.value().Opcode);
        EXPECT_EQ(AddressingMode::Register, ld.InstructionData.value().AddressingMode);
        EXPECT_EQ(source, ld.InstructionData.value().SourceRegister);
        EXPECT_EQ(destination, ld.InstructionData.value().DestinationRegister);
    }

    auto invalidDestination = {Register::F};
    auto invalidSource = {Register::H};

    for (auto source : sourcesList)
    for (auto destination : invalidDestination)
    {
        auto testPassed = false;

        try
        {
            LD ld;
            auto rawBinary = BinaryRegisterAddressingMode(0x01, source, destination);
            ld.Decode(rawBinary);
        }
        catch (const RegisterBankException& e)
        {
            testPassed = true;
        }

        EXPECT_TRUE(testPassed);
    }

    for (auto destination : destinationsList)
    for (auto source : invalidSource)
    {
        auto testPassed = false;

        try
        {
            LD ld;
            auto rawBinary = BinaryRegisterAddressingMode(0x01, source, destination);
            ld.Decode(rawBinary);
        }
        catch (const RegisterBankException& e)
        {
            testPassed = true;
        }

        EXPECT_TRUE(testPassed);
    }

    for (auto destination : destinationsList)
    {
        auto fobiddenSourceTest = false;

        try
        {
            LD ld;
            auto bin = 0x05 | 
                       RegisterBank::ToInstructionDestination(destination) << 3 |
                       0x01 << 6;

            ld.Decode(bin);
        }
        catch (const RegisterBankException& e)
        {
            fobiddenSourceTest = true;
        }

        EXPECT_TRUE(fobiddenSourceTest);
    }

    for (auto source : sourcesList)
    {
        auto fobiddenDestinationTest = false;

        try
        {
            LD ld;
            auto bin = RegisterBank::ToInstructionSource(source) | 
                       0x05 << 3 |
                       0x01 << 6;

            ld.Decode(bin);
        }
        catch (const RegisterBankException& e)
        {
            fobiddenDestinationTest = true;
        }

        EXPECT_TRUE(fobiddenDestinationTest);
    }
}

TEST(TestLD, DecodeInvalidLDInstruction)
{
}