#include <gtest/gtest.h>

#include <array>
#include <cstdlib>
#include <memory>
#include <optional>
#include <variant>

#include "../src/GBXExceptions.h"
#include "../src/ROM.h"
#include "../src/interfaces/MemoryControllerInterface.h"

using namespace std;
using namespace gbx;
using namespace gbx::interfaces;

TEST(TestROM, Construction) 
{
    ROM rom(static_cast<size_t>(100));
    auto size = rom.Size();

    EXPECT_EQ(static_cast<size_t>(100), size);
}

TEST(TestROM, CheckMemoryInitialization) 
{
    ROM rom(static_cast<size_t>(100));

    for (uint16_t i = 0; i < static_cast<uint16_t>(rom.Size()); i++)
    {
        auto value = rom.Read(i, MemoryAccessType::Byte);
        EXPECT_EQ(0x00, get<uint8_t>(value));
    }
}

TEST(TestROM, WriteByte)
{
    ROM rom(static_cast<size_t>(100));
    rom.Write(static_cast<uint8_t>(0xAA), 0x0020);
    auto value = rom.Read(0x0020, MemoryAccessType::Byte);
    EXPECT_EQ(0xAA, get<uint8_t>(value));
}

TEST(TestROM, WriteWord)
{
    ROM rom(static_cast<size_t>(100));
    rom.Write(static_cast<uint16_t>(0xFFEE), 0x0030);
    auto value = rom.Read(0x0030, MemoryAccessType::Word);
    EXPECT_EQ(0xFFEE, get<uint16_t>(value));
}

TEST(TestROM, ReadInvalidAddresses)
{
    ROM rom(static_cast<size_t>(0x50));
    auto invalidByteAddress = false;
    auto invalidWordAddress = false;

    try
    {
        rom.Read(0x0051, MemoryAccessType::Byte);
    }
    catch (const MemoryAccessException& e)
    {
        invalidByteAddress = true;
    }

    try
    {
        rom.Read(0x0050,  MemoryAccessType::Word);
    }
    catch (const MemoryAccessException& e)
    {
        invalidWordAddress = true;
    }

    EXPECT_TRUE(invalidByteAddress);
    EXPECT_TRUE(invalidWordAddress);
}

TEST(TestROM, ReadInvalidAddressesTest2)
{
    ROM rom(static_cast<size_t>(0x50));
    auto invalidByteAddress = false;
    auto invalidWordAddress = false;

    try
    {
        rom.Read(0x0100,  MemoryAccessType::Byte);
    }
    catch (const MemoryAccessException& e)
    {
        invalidByteAddress = true;
    }

    try
    {
        rom.Read(0x0101,  MemoryAccessType::Word);
    }
    catch (const MemoryAccessException& e)
    {
        invalidWordAddress = true;
    }

    EXPECT_TRUE(invalidByteAddress);
    EXPECT_TRUE(invalidWordAddress);
}

TEST(TestROM, WriteInvalidAddresses)
{
    ROM rom(static_cast<size_t>(0x50));
    auto invalidByteAddress = false;
    auto invalidWordAddress = false;

    try
    {
        rom.Write(static_cast<uint8_t>(0x91), 0x0050);
    }
    catch (const MemoryAccessException& e)
    {
        invalidByteAddress = true;
    }

    try
    {
        rom.Write(static_cast<uint16_t>(0xFFFF), 0x0051);
    }
    catch (const MemoryAccessException& e)
    {
        invalidWordAddress = true;
    }

    EXPECT_TRUE(invalidByteAddress);
    EXPECT_TRUE(invalidWordAddress);
}

TEST(TestROM, WriteInvalidAddressesTest2)
{
    ROM rom(static_cast<size_t>(0x50));
    auto invalidByteAddress = false;
    auto invalidWordAddress = false;

    try
    {
        rom.Write(static_cast<uint8_t>(0x10), 0x0100);
    }
    catch (const MemoryAccessException& e)
    {
        invalidByteAddress = true;
    }

    try
    {
        rom.Write(static_cast<uint16_t>(0x01), 0x0100);
    }
    catch (const MemoryAccessException& e)
    {
        invalidWordAddress = true;
    }

    EXPECT_TRUE(invalidByteAddress);
    EXPECT_TRUE(invalidWordAddress);
}

TEST(TestROM, LoadROM)
{
    ROM rom(static_cast<size_t>(0x10));
    array<uint8_t, 0x10> romContent = {0x0F, 0x0E, 0x0D, 0x0C, 
                                       0x0B, 0x0A, 0x09, 0x08, 
                                       0x07, 0x06, 0x05, 0x04, 
                                       0x03, 0x02, 0x01, 0x00};

    rom.Load(make_shared<uint8_t*>(romContent.data()), 0x10, std::nullopt);

    uint16_t address = 0x00;
    for(auto element : romContent)
    {
        auto value = rom.Read(address++, MemoryAccessType::Byte);
        ASSERT_EQ(element, get<uint8_t>(value));
    }
}

TEST(TestROM, LoadWithOffset)
{
    ROM rom(static_cast<size_t>(0x10));
    array<uint8_t, 0x10> romContent = {0x0F, 0x0E, 0x0D, 0x0C, 
                                       0x0B, 0x0A, 0x09, 0x08, 
                                       0x07, 0x06, 0x05, 0x04, 
                                       0x03, 0x02, 0x01, 0x00};

    array<uint8_t, 0x08> offsetContent = {0xAA, 0xAA, 0xAA, 0xAA, 
                                          0xAA, 0xAA, 0xAA, 0xAA};

    rom.Load(make_shared<uint8_t*>(romContent.data()), 0x10, std::nullopt);
    rom.Load(make_shared<uint8_t*>(offsetContent.data()), 0x08, make_optional<size_t>(0x08));

    for(auto i = static_cast<size_t>(0); i < rom.Size(); i++)
    {
        auto value = rom.Read(i, MemoryAccessType::Byte);
        if (i < 0x08)
            ASSERT_EQ(romContent[i], get<uint8_t>(value));
        else
            ASSERT_EQ(offsetContent[i - 0x08], get<uint8_t>(value));
    }
}

TEST(TestROM, LoadOffsetOutOfBounds)
{
    auto testPassed = false;
    ROM rom(static_cast<size_t>(0x10));

    array<uint8_t, 0x08> offsetContent = {0xAA, 0xAA, 0xAA, 0xAA, 
                                          0xAA, 0xAA, 0xAA, 0xAA};

    try
    {
        rom.Load(make_shared<uint8_t*>(offsetContent.data()), 0x08, make_optional<size_t>(0x0A));
    }
    catch (const MemoryAccessException& e)
    {
        testPassed = true;
    }

    EXPECT_TRUE(testPassed);
}