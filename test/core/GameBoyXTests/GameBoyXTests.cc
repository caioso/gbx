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
#include "SystemMode.h"

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

std::string TooLargegSampleMultipleGameFileName()
{
    return GBXTestEnvironment::TestDataPath + "too_large.gbc";
}

std::string BIOSFileName()
{
    return GBXTestEnvironment::TestDataPath + "bios.gbx";
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

    // Change to user mode to be able to see the loaded game content;
    gbx.SetMode(Mode::User);

    // Check fixed bank 1
    for (auto i = DMGBCFixedROMInitialAddress; i < DMGBCFixedROMFinalAddress; ++i)
        EXPECT_EQ(fileContent[i], get<uint8_t>(gbx.ReadROM(static_cast<uint16_t>(i), nullopt, MemoryAccessType::Byte)));
    
    // Check dynamic bank 1
    for (auto i = DMGBCBankedROMInitialAddress; i < size; ++i)
        EXPECT_EQ(fileContent[i], get<uint8_t>(gbx.ReadROM(static_cast<uint16_t>(i), nullopt, MemoryAccessType::Byte)));
}

TEST(CoreTests_GameBoyXTests, LoadGameMultipleBankGame) 
{
    FileLoader loader(SampleMultipleGameFileName());
    GameBoyX gbx;
    
    gbx.LoadGame(SampleMultipleGameFileName());
    loader.Load();
    auto [fileContent, size] = loader.FileData();

    // Change to user mode to be able to see the loaded game content;
    gbx.SetMode(Mode::User);

    // Check fixed bank 0
    for (auto i = DMGBCFixedROMInitialAddress; i < DMGBCFixedROMFinalAddress; ++i)
        EXPECT_EQ(fileContent[i], get<uint8_t>(gbx.ReadROM(static_cast<uint16_t>(i), nullopt, MemoryAccessType::Byte)));
    
    // Check dynamic bank 1-128
    for (auto j = 0llu; j < (size - DMGBCROMBankSize)/DMGBCROMBankSize; ++j)
        for (auto i = DMGBCBankedROMInitialAddress; i < DMGBCBankedROMFinalAddress; ++i)
            EXPECT_EQ(fileContent[DMGBCROMBankSize*j + i], get<uint8_t>(gbx.ReadROM(static_cast<uint16_t>(i), j, MemoryAccessType::Byte)));
}

TEST(CoreTests_GameBoyXTests, LoadSystemBIOS)
{
    FileLoader systemBIOSLoader(BIOSFileName());
    GameBoyX gbx;

    gbx.LoadBIOS(BIOSFileName());
    gbx.SetMode(Mode::System);
    
    systemBIOSLoader.Load();
    auto [fileContent, size] = systemBIOSLoader.FileData();

    // Check fixed bank 1
    for (auto i = GBXSystemROMInitialAddress; i < size; ++i)
        EXPECT_EQ(fileContent[i], get<uint8_t>(gbx.ReadROM(static_cast<uint16_t>(i), nullopt, MemoryAccessType::Byte)));
}

TEST(CoreTests_GameBoyXTests, LoadTooLargeBIOS)
{
    GameBoyX gbx;
    ASSERT_EXCEPTION( { gbx.LoadBIOS(TooLargegSampleMultipleGameFileName()); }, 
                      MemoryControllerException, 
                      "BIOS file larger than 32KB");    
}

TEST(CoreTests_GameBoyXTests, LoadTooLargeGameROM)
{
    GameBoyX gbx;
    ASSERT_EXCEPTION( { gbx.LoadGame(TooLargegSampleMultipleGameFileName()); }, 
                      MemoryControllerException, 
                      "Game ROM file larger than 2MB");    
}