#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <chrono>
#include <thread>
#include <memory>

#include "TestUtils.h"

#include "FileLoader.h"
#include "GameBoyX.h"
#include "GBXCoreExceptions.h"
#include "MemoryController.h"
#include "SystemConstants.h"

using namespace std;
using namespace gbxcore;
using namespace gbxcore::constants;
using namespace gbxcore::interfaces;
using namespace gbxcommons;

std::string SampleGameFileName()
{
    return GBXTestEnvironment::TestDataPath + "rom.gb";
}

std::string SampleMultipleGameFileName()
{
    return GBXTestEnvironment::TestDataPath + "rom.gbc";
}

TEST(CoreTests_GameBoyXTests, Construction) 
{
    GameBoyX gbx;    
}

TEST(CoreTests_GameBoyXTests, LoadGame) 
{
    FileLoader loader(SampleGameFileName());
    GameBoyX gbx;
    
    gbx.LoadGame(SampleGameFileName());
    loader.Load();
    auto [fileContent, size] = loader.FileData();

    // Check Fixed Bank Size
    for (auto i = UserFixedROMInitialAddress; i < UserFixedROMFinalAddress; ++i)
        EXPECT_EQ(fileContent[i], get<uint8_t>(gbx.ReadROM(static_cast<uint16_t>(i), nullopt, MemoryAccessType::Byte)));
    
    // Check dynamic bank 1
    for (auto i = UserBankedROMInitialAddress; i < size; ++i)
        EXPECT_EQ(fileContent[i], get<uint8_t>(gbx.ReadROM(static_cast<uint16_t>(i), nullopt, MemoryAccessType::Byte)));
}

TEST(CoreTests_GameBoyXTests, LoadGameMultipleBankGame) 
{
    FileLoader loader(SampleMultipleGameFileName());
    GameBoyX gbx;
    
    gbx.LoadGame(SampleMultipleGameFileName());
    loader.Load();
    auto [fileContent, size] = loader.FileData();

    // Check Fixed Bank Size (bank 0)
    for (auto i = UserFixedROMInitialAddress; i < UserFixedROMFinalAddress; ++i)
        EXPECT_EQ(fileContent[i], get<uint8_t>(gbx.ReadROM(static_cast<uint16_t>(i), nullopt, MemoryAccessType::Byte)));
    
    // Check dynamic bank 1-128
    for (auto j = 0; j < (size - DefaultROMBankSize)/DefaultROMBankSize; ++j)
        for (auto i = UserBankedROMInitialAddress; i < UserBankedROMFinalAddress; ++i)
            EXPECT_EQ(fileContent[DefaultROMBankSize*j + i], get<uint8_t>(gbx.ReadROM(static_cast<uint16_t>(i), j, MemoryAccessType::Byte)));
}