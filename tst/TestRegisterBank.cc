#include <gtest/gtest.h>
#include <array>

#include "../src/RegisterBank.h"

using namespace std;
using namespace gbx;

TEST(RegisterBankTests, ReadRegisterA) 
{
    RegisterBank bank;
    auto value = bank.Read(Register::A);
    EXPECT_EQ(0, value);
}

TEST(RegisterBankTests, WriteRegisterA) 
{
    RegisterBank bank;
    bank.Write(Register::A, 0x01);
    auto value = bank.Read(Register::A);
    EXPECT_EQ(1, value);
}

TEST(RegisterBankTests, WriteRegisterF) 
{
    RegisterBank bank;
    bank.Write(Register::F, 0x98);
    auto value = bank.Read(Register::F);
    EXPECT_EQ(0x98, value);
}

TEST(RegisterBankTests, WriteRegisterHL) 
{
    RegisterBank bank;
    bank.WritePair(Register::HL, 0xAABB);
    auto value = bank.ReadPair(Register::HL);
    EXPECT_EQ(0xAABB, value);
}

TEST(RegisterBankTests, WriteRegisterPC) 
{
    RegisterBank bank;
    bank.WritePair(Register::PC, 0xDDCC);
    auto value = bank.ReadPair(Register::PC);
    EXPECT_EQ(0xDDCC, value);
}

TEST(RegisterBankTests, WriteRegisterSP) 
{
    RegisterBank bank;
    bank.WritePair(Register::SP, 0xABCD);
    auto value = bank.ReadPair(Register::SP);
    EXPECT_EQ(0xABCD, value);
}

TEST(RegisterBankTests, WriteRegisterAll8BitRegisters) 
{
    RegisterBank bank;
    array<Register, 7> registers = { Register::B, Register::C, Register::D, Register::E,
                                     Register::H, Register::L, Register::A };

    for (auto it = begin(registers); it != end(registers); ++it)
    {
        auto value = static_cast<uint8_t>(distance(registers.begin(), it));
        bank.Write(*it, value);

        ASSERT_EQ(value, bank.Read(*it));
    }    
}

TEST(RegisterBankTests, WriteRegisterAll16BitRegisters) 
{
    RegisterBank bank;
    array<Register, 4> registers = { Register::HL, Register::BC, Register::DE, Register::AF };

    for (auto it = begin(registers); it != end(registers); ++it)
    {
        auto value = static_cast<uint8_t>(distance(registers.begin(), it)) + 1;
        bank.WritePair(*it, ((value<<8) | value));

        ASSERT_EQ(((value<<8) | value), bank.ReadPair(*it));
    }    
}
