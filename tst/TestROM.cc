#include <gtest/gtest.h>

#include <array>
#include <cstdlib>
#include <memory>
#include <optional>

#include "../src/GBXExceptions.h"
#include "../src/ROM.h"

using namespace std;
using namespace gbx;

TEST(ROMTests, Construction) 
{
    ROM rom(static_cast<size_t>(100));
    auto size = rom.Size();

    EXPECT_EQ(static_cast<size_t>(100), size);
}

TEST(ROMTests, CheckMemoryInitialization) 
{
    ROM rom(static_cast<size_t>(100));

    for (uint16_t i = 0; i < static_cast<uint16_t>(rom.Size()); i++)
    {
        auto value = rom.ReadByte(i);
        EXPECT_EQ(0x00, value);
    }
}

TEST(ROMTests, WriteByte)
{
    ROM rom(static_cast<size_t>(100));
    rom.WriteByte(0xAA, 0x0020);
    auto value = rom.ReadByte(0x0020);

    EXPECT_EQ(0xAA, value);
}

TEST(ROMTests, WriteWord)
{
    ROM rom(static_cast<size_t>(100));
    rom.WriteWord(0xFFAA, 0x0030);
    auto value = rom.ReadWord(0x0030);

    EXPECT_EQ(0xFFAA, value);
}

TEST(ROMTests, ReadInvalidAddresses)
{
    ROM rom(static_cast<size_t>(0x50));
    auto invalidByteAddress = false;
    auto invalidWordAddress = false;

    try
    {
        rom.ReadByte(0x0051);
    }
    catch (const MemoryAccessException& e)
    {
        invalidByteAddress = true;
    }

    try
    {
        rom.ReadWord(0x0050);
    }
    catch (const MemoryAccessException& e)
    {
        invalidWordAddress = true;
    }

    EXPECT_TRUE(invalidByteAddress);
    EXPECT_TRUE(invalidWordAddress);
}

TEST(ROMTests, ReadInvalidAddressesTest2)
{
    ROM rom(static_cast<size_t>(0x50));
    auto invalidByteAddress = false;
    auto invalidWordAddress = false;

    try
    {
        rom.ReadByte(0x0100);
    }
    catch (const MemoryAccessException& e)
    {
        invalidByteAddress = true;
    }

    try
    {
        rom.ReadWord(0x0101);
    }
    catch (const MemoryAccessException& e)
    {
        invalidWordAddress = true;
    }

    EXPECT_TRUE(invalidByteAddress);
    EXPECT_TRUE(invalidWordAddress);
}

TEST(ROMTests, WriteInvalidAddresses)
{
    ROM rom(static_cast<size_t>(0x50));
    auto invalidByteAddress = false;
    auto invalidWordAddress = false;

    try
    {
        rom.WriteByte(0x91, 0x0050);
    }
    catch (const MemoryAccessException& e)
    {
        invalidByteAddress = true;
    }

    try
    {
        rom.WriteWord(0xFFFF, 0x0051);
    }
    catch (const MemoryAccessException& e)
    {
        invalidWordAddress = true;
    }

    EXPECT_TRUE(invalidByteAddress);
    EXPECT_TRUE(invalidWordAddress);
}

TEST(ROMTests, WriteInvalidAddressesTest2)
{
    ROM rom(static_cast<size_t>(0x50));
    auto invalidByteAddress = false;
    auto invalidWordAddress = false;

    try
    {
        rom.WriteByte(0x10, 0x0100);
    }
    catch (const MemoryAccessException& e)
    {
        invalidByteAddress = true;
    }

    try
    {
        rom.WriteWord(0x01, 0x0100);
    }
    catch (const MemoryAccessException& e)
    {
        invalidWordAddress = true;
    }

    EXPECT_TRUE(invalidByteAddress);
    EXPECT_TRUE(invalidWordAddress);
}

TEST(ROMTests, LoadROM)
{
    ROM rom(static_cast<size_t>(0x10));
    array<uint8_t, 0x10> romContent = {0x0F, 0x0E, 0x0D, 0x0C, 
                                       0x0B, 0x0A, 0x09, 0x08, 
                                       0x07, 0x06, 0x05, 0x04, 
                                       0x03, 0x02, 0x01, 0x00};

    rom.Load(romContent.data(), 0x10, std::nullopt);

    uint16_t address = 0x00;
    for(auto element : romContent)
    {
        auto value = rom.ReadByte(address++);
        ASSERT_EQ(element, value);
    }
}