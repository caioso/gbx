#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestUtils.h"

#include <chrono>
#include <memory>
#include <thread>
#include <variant>

#include "BankedROM.h"
#include "FileLoader.h"
#include "GBXCoreExceptions.h"

using namespace std;
using namespace gbxcommons;
using namespace gbxcore;
using namespace gbxcore::interfaces;
using namespace gbxcore::memory;

TEST(CoreTests_BankedROMAndROM, ContructionBankedROM) 
{
    // 1024 real memory size, 128 segment size
    BankedROM bankedROM(1024llu, 128llu);

    EXPECT_EQ(1024llu, bankedROM.PhysicalResourceSize());
    EXPECT_EQ(128llu, bankedROM.Size());
    EXPECT_EQ(128llu, bankedROM.BankSize());
    EXPECT_EQ(8llu, bankedROM.BankCount());
    EXPECT_EQ(0llu, bankedROM.CurrentBank());
}

TEST(CoreTests_BankedROMAndROM, ContructionChangeCurrentMemoryBank) 
{
    BankedROM bankedROM(2048llu, 128llu);

    EXPECT_EQ(2048llu, bankedROM.PhysicalResourceSize());
    EXPECT_EQ(128llu, bankedROM.Size());
    EXPECT_EQ(128llu, bankedROM.BankSize());
    EXPECT_EQ(16llu, bankedROM.BankCount());
    EXPECT_EQ(0llu, bankedROM.CurrentBank());

    for (auto i = 0llu; i < 16llu; ++i)
    {
        bankedROM.SelectBank(i);
        EXPECT_EQ(i, bankedROM.CurrentBank());
    }
}

TEST(CoreTests_BankedROMAndROM, ContructionSelectInvalidBank) 
{
    BankedROM bankedROM(4096llu, 64llu);

    EXPECT_EQ(4096llu, bankedROM.PhysicalResourceSize());
    EXPECT_EQ(64llu, bankedROM.Size());
    EXPECT_EQ(64llu, bankedROM.BankSize());
    EXPECT_EQ(64llu, bankedROM.BankCount());
    EXPECT_EQ(0llu, bankedROM.CurrentBank());

    ASSERT_EXCEPTION( { bankedROM.SelectBank(0xFFFF); }, 
                      BankedMemoryException, 
                      "Invalid memory bank '65535' selected");
}

TEST(CoreTests_BankedROMAndROM, ReadFromBankedROM) 
{
    FileLoader loader(GBXTestEnvironment::TestDataPath + "banked_memory_resource.bin");
    BankedROM bankedROM(2048, 128llu);

    EXPECT_EQ(2048llu, bankedROM.PhysicalResourceSize());
    EXPECT_EQ(128llu, bankedROM.Size());
    EXPECT_EQ(128llu, bankedROM.BankSize());
    EXPECT_EQ(16llu, bankedROM.BankCount());
    EXPECT_EQ(0llu, bankedROM.CurrentBank());

    loader.Load();
    auto [fileBytes, size] = loader.FileData();

    bankedROM.Load(make_shared<uint8_t*>(fileBytes.get()), size, nullopt);

    auto expectedBytes = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0xEE};
    for (auto bank = 0llu; bank < 16llu; ++bank)
    {
        bankedROM.SelectBank(bank);
        
        for (auto byte = 0llu; byte < bankedROM.BankSize(); ++byte)
            EXPECT_EQ(*(expectedBytes.begin() + bank), get<uint8_t>(bankedROM.Read(byte, MemoryAccessType::Byte)));
    }
}

TEST(CoreTests_BankedROMAndROM, ReadFromBankedROMOutsideOfBoundaries) 
{
    BankedROM bankedROM(2048, 128llu);

    EXPECT_EQ(2048llu, bankedROM.PhysicalResourceSize());
    EXPECT_EQ(128llu, bankedROM.Size());
    EXPECT_EQ(128llu, bankedROM.BankSize());
    EXPECT_EQ(16llu, bankedROM.BankCount());
    EXPECT_EQ(0llu, bankedROM.CurrentBank());

    bankedROM.SelectBank(0x04);

    ASSERT_EXCEPTION( { bankedROM.Read(139, MemoryAccessType::Byte); }, 
                      BankedMemoryException, 
                      "Address '139' is out of bank '4' bounds (bank size = 128)" );
}

TEST(CoreTests_BankedROMAndROM, ReadFromBankedROMOutsideOfBoundaries2) 
{
    BankedROM bankedROM(2048, 128llu);

    EXPECT_EQ(2048llu, bankedROM.PhysicalResourceSize());
    EXPECT_EQ(128llu, bankedROM.Size());
    EXPECT_EQ(128llu, bankedROM.BankSize());
    EXPECT_EQ(16llu, bankedROM.BankCount());
    EXPECT_EQ(0llu, bankedROM.CurrentBank());

    bankedROM.SelectBank(0x0F);

    ASSERT_EXCEPTION( { bankedROM.Read(128, MemoryAccessType::Byte); }, 
                      BankedMemoryException, 
                      "Address '128' is out of bank '15' bounds (bank size = 128)");
}

TEST(CoreTests_BankedROMAndROM, ReadFromBankedROMOutsideOfBoundariesWordMode) 
{
    BankedROM bankedROM(2048, 128llu);

    EXPECT_EQ(2048llu, bankedROM.PhysicalResourceSize());
    EXPECT_EQ(128llu, bankedROM.Size());
    EXPECT_EQ(128llu, bankedROM.BankSize());
    EXPECT_EQ(16llu, bankedROM.BankCount());
    EXPECT_EQ(0llu, bankedROM.CurrentBank());

    bankedROM.SelectBank(0x07);

    ASSERT_EXCEPTION( { bankedROM.Read(127, MemoryAccessType::Word); }, 
                      BankedMemoryException, 
                      "Addresses '127' and '128' are out of bank '7' bounds (bank size = 128)");
}

TEST(CoreTests_BankedROMAndROM, WriteToROM) 
{
    BankedROM bankedROM(2048, 128llu);

    EXPECT_EQ(2048llu, bankedROM.PhysicalResourceSize());
    EXPECT_EQ(128llu, bankedROM.Size());
    EXPECT_EQ(128llu, bankedROM.BankSize());
    EXPECT_EQ(16llu, bankedROM.BankCount());
    EXPECT_EQ(0llu, bankedROM.CurrentBank());

    bankedROM.SelectBank(0x02);

    ASSERT_EXCEPTION( { bankedROM.Write(static_cast<uint8_t>(0xFF), 0x0000); }, 
                      MemoryAccessException, 
                      "Attempted to write to a read-only resource");
}

TEST(CoreTests_BankedROMAndROM, WriteToROM2) 
{
    BankedROM bankedROM(2048, 128llu);

    EXPECT_EQ(2048llu, bankedROM.PhysicalResourceSize());
    EXPECT_EQ(128llu, bankedROM.Size());
    EXPECT_EQ(128llu, bankedROM.BankSize());
    EXPECT_EQ(16llu, bankedROM.BankCount());
    EXPECT_EQ(0llu, bankedROM.CurrentBank());

    bankedROM.SelectBank(0x02);

    ASSERT_EXCEPTION( { bankedROM.Write(static_cast<uint8_t>(0xFF), 0x00FF); }, 
                      BankedMemoryException, 
                      "Address '255' is out of bank '2' bounds (bank size = 128)");
}
