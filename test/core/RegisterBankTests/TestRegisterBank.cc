#include <gtest/gtest.h>
#include <array>

#include "RegisterBank.h"

using namespace std;
using namespace gbxcore;
using namespace gbxcore::interfaces;

TEST(TestRegisterBank, ReadRegisterA) 
{
    RegisterBank bank;
    auto value = bank.Read(Register::A);
    EXPECT_EQ(0, value);
}

TEST(TestRegisterBank, WriteRegisterA) 
{
    RegisterBank bank;
    bank.Write(Register::A, 0x01);
    auto value = bank.Read(Register::A);
    EXPECT_EQ(1, value);
}

TEST(TestRegisterBank, WriteRegisterF) 
{
    RegisterBank bank;
    bank.Write(Register::F, 0x98);
    auto value = bank.Read(Register::F);
    EXPECT_EQ(0x98, value);
}

TEST(TestRegisterBank, WriteRegisterHL) 
{
    RegisterBank bank;
    bank.WritePair(Register::HL, 0xAABB);
    auto value = bank.ReadPair(Register::HL);
    EXPECT_EQ(0xAABB, value);
}

TEST(TestRegisterBank, WriteRegisterPC) 
{
    RegisterBank bank;
    bank.WritePair(Register::PC, 0xDDCC);
    auto value = bank.ReadPair(Register::PC);
    EXPECT_EQ(0xDDCC, value);
}

TEST(TestRegisterBank, WriteRegisterSP) 
{
    RegisterBank bank;
    bank.WritePair(Register::SP, 0xABCD);
    auto value = bank.ReadPair(Register::SP);
    EXPECT_EQ(0xABCD, value);
}

TEST(TestRegisterBank, WriteRegisterAll8BitRegisters) 
{
    RegisterBank bank;
    array<Register, 7> registers = { Register::B, Register::C, Register::D, Register::E,
                                     Register::H, Register::L, Register::A };

    for (auto it = begin(registers); it != end(registers); ++it)
    {
        auto value = static_cast<uint8_t>(distance(registers.begin(), it));
        bank.Write(*it, value);

        EXPECT_EQ(value, bank.Read(*it));
    }    
}

TEST(TestRegisterBank, WriteRegisterAll16BitRegisters) 
{
    RegisterBank bank;
    array<Register, 4> registers = { Register::HL, Register::BC, Register::DE, Register::AF };

    for (auto it = begin(registers); it != end(registers); ++it)
    {
        auto value = static_cast<uint8_t>(distance(registers.begin(), it)) + 1;
        bank.WritePair(*it, ((value<<8) | value));

        EXPECT_EQ(((value<<8) | value), bank.ReadPair(*it));
    }    
}

TEST(TestRegisterBank, InstructionSourceToRegister)
{
    RegisterBank bank;
    
    EXPECT_EQ(0x00, bank.ToInstructionSource(Register::B));
    EXPECT_EQ(0x01, bank.ToInstructionSource(Register::C));
    EXPECT_EQ(0x02, bank.ToInstructionSource(Register::D));
    EXPECT_EQ(0x03, bank.ToInstructionSource(Register::E));
    EXPECT_EQ(0x04, bank.ToInstructionSource(Register::H));
    EXPECT_EQ(0x05, bank.ToInstructionSource(Register::L));
    EXPECT_EQ(0x07, bank.ToInstructionSource(Register::A));

    array<Register, 12> nonSources = { Register::F, Register::HL, Register::PIR, Register::IR, Register::IX, Register::IY,
                                       Register::PC, Register::R, Register::SP, Register::AF, Register::BC, Register::DE };
    
    for (auto reg : nonSources)
    {
        auto checkPassed = false;
        try
        {
            bank.ToInstructionSource(reg);
        }
        catch (const RegisterBankException& e)
        {
            checkPassed = true;
        }
        
        EXPECT_TRUE(checkPassed);
        checkPassed = false;
    }
}

TEST(TestRegisterBank, InstructionDestinationToRegister)
{
    RegisterBank bank;
    
    EXPECT_EQ(0x00, bank.ToInstructionDestination(Register::B));
    EXPECT_EQ(0x01, bank.ToInstructionDestination(Register::C));
    EXPECT_EQ(0x02, bank.ToInstructionDestination(Register::D));
    EXPECT_EQ(0x03, bank.ToInstructionDestination(Register::E));
    EXPECT_EQ(0x04, bank.ToInstructionDestination(Register::H));
    EXPECT_EQ(0x05, bank.ToInstructionDestination(Register::L));
    EXPECT_EQ(0x07, bank.ToInstructionDestination(Register::A));

    array<Register, 12> nonSources = { Register::F, Register::HL, Register::PIR, Register::IR, Register::IX, Register::IY,
                                       Register::PC, Register::R, Register::SP, Register::AF, Register::BC, Register::DE };
    
    for (auto reg : nonSources)
    {
        auto checkPassed = false;
        try
        {
            bank.ToInstructionDestination(reg);
        }
        catch (const RegisterBankException& e)
        {
            checkPassed = true;
        }
        
        EXPECT_TRUE(checkPassed);
        checkPassed = false;
    }
}

TEST(TestRegisterBank, FromInstructionSourceToRegister)
{
    RegisterBank bank;
    
    EXPECT_EQ(Register::B, bank.FromInstructionSource(0x00));
    EXPECT_EQ(Register::C, bank.FromInstructionSource(0x01));
    EXPECT_EQ(Register::D, bank.FromInstructionSource(0x02));
    EXPECT_EQ(Register::E, bank.FromInstructionSource(0x03));
    EXPECT_EQ(Register::H, bank.FromInstructionSource(0x04));
    EXPECT_EQ(Register::L, bank.FromInstructionSource(0x05));
    EXPECT_EQ(Register::A, bank.FromInstructionSource(0x07));
    
    for (auto i = 8; i < 255; i++)
    {
        auto checkPassed = false;
        try
        {
            bank.FromInstructionSource(i);
        }
        catch (const RegisterBankException& e)
        {
            checkPassed = true;
        }
        
        EXPECT_TRUE(checkPassed);
        checkPassed = false;
    }
}

TEST(TestRegisterBank, FromInstructionDestinationToRegister)
{
    RegisterBank bank;
    
    EXPECT_EQ(Register::B, bank.FromInstructionDestination(0x00));
    EXPECT_EQ(Register::C, bank.FromInstructionDestination(0x01));
    EXPECT_EQ(Register::D, bank.FromInstructionDestination(0x02));
    EXPECT_EQ(Register::E, bank.FromInstructionDestination(0x03));
    EXPECT_EQ(Register::H, bank.FromInstructionDestination(0x04));
    EXPECT_EQ(Register::L, bank.FromInstructionDestination(0x05));
    EXPECT_EQ(Register::A, bank.FromInstructionDestination(0x07));
    
    for (auto i = 8; i < 255; i++)
    {
        auto checkPassed = false;
        try
        {
            bank.FromInstructionDestination(i);
        }
        catch (const RegisterBankException& e)
        {
            checkPassed = true;
        }
        
        EXPECT_TRUE(checkPassed);
        checkPassed = false;
    }
}

TEST(TestRegisterBank, TestFlags)
{
    RegisterBank bank;

    auto flags = {Flag::CY, Flag::H, Flag::N, Flag::Z};

    for (auto flag : flags)
        EXPECT_EQ(0x00, bank.ReadFlag(flag));

    for (auto flag : flags)
        bank.SetFlag(flag);

    for (auto flag : flags)
        EXPECT_EQ(0x01, bank.ReadFlag(flag));

    for (auto flag : flags)
        bank.ClearFlag(flag);

    for (auto flag : flags)
        EXPECT_EQ(0x00, bank.ReadFlag(flag));

    for (auto flag : flags)
        bank.WriteFlag(flag, 0x00);

    for (auto flag : flags)
        EXPECT_EQ(0x00, bank.ReadFlag(flag));
    
    for (auto flag : flags)
        bank.WriteFlag(flag, 0x01);

    for (auto flag : flags)
        EXPECT_EQ(0x01, bank.ReadFlag(flag));
}
