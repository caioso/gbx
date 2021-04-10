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

std::string SampleInvalidLogoGameFileName()
{
    return GBXTestEnvironment::TestDataPath + "invalid_logo_rom.gb";
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

TEST(CoreTests_GameBoyXTests, ExecuteSystemBIOS)
{
    // This test runs the instructions neeed to execute the 'Nintendo Logo checking' during the system bootup
    auto nintendoLogoBytes = {0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b, 
                              0x03, 0x73, 0x00, 0x83, 0x00, 0x0c, 0x00, 0x0d, 
                              0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e, 
                              0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99, 
                              0xbb, 0xbb, 0x67, 0x63, 0x6e, 0x0e, 0xec, 0xcc, 
                              0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e};
    GameBoyX gbx;
    gbx.LoadBIOS(BIOSFileName());
    gbx.LoadGame(SampleGameFileName());
    gbx.SetMode(Mode::System);

    // Run NOP
    gbx.Run();
    EXPECT_EQ(0x00, get<uint8_t>(gbx.ReadRegister(Register::IR)));

    // LD HL, 0x0200   
    gbx.Run();
    EXPECT_EQ(0x0200, get<uint16_t>(gbx.ReadRegister(Register::HL)));
    
    // LD DE, 0x0104
    gbx.Run();
    EXPECT_EQ(0x0104, get<uint16_t>(gbx.ReadRegister(Register::DE)));
    
    // LD C, 0x30
    gbx.Run();
    EXPECT_EQ(0x30, get<uint8_t>(gbx.ReadRegister(Register::C)));
    
    // Loop
    for (auto i = 0llu; i < nintendoLogoBytes.size(); ++i)
    {
        // LD A, [HL+]
        gbx.Run();
        EXPECT_EQ((*(begin(nintendoLogoBytes) + i)), get<uint8_t>(gbx.ReadRegister(Register::A)));
        EXPECT_EQ(0x0200 + i + 1, get<uint16_t>(gbx.ReadRegister(Register::HL)));

        // LD B, A
        gbx.Run();
        EXPECT_EQ((*(begin(nintendoLogoBytes) + i)), get<uint8_t>(gbx.ReadRegister(Register::B)));
        
        // LDU A, [DE]
        gbx.Run();
        EXPECT_EQ((*(begin(nintendoLogoBytes) + i)), get<uint8_t>(gbx.ReadRegister(Register::A)));

        // INC DE
        gbx.Run();
        EXPECT_EQ(0x0104 + i + 1, get<uint16_t>(gbx.ReadRegister(Register::DE)));
        
        // CP B
        gbx.Run();
        EXPECT_EQ(0x80, 0x80 & get<uint8_t>(gbx.ReadRegister(Register::F)));
        
        // JP NZ, 0x0150
        gbx.Run();
        EXPECT_NE(0x0150, get<uint16_t>(gbx.ReadRegister(Register::PC)));
        
        // DEC C
        gbx.Run();
        EXPECT_EQ(0x30 - (i + 1), get<uint8_t>(gbx.ReadRegister(Register::C)));
        
        // LD A, 0x00
        gbx.Run();
        EXPECT_EQ(0x00, get<uint8_t>(gbx.ReadRegister(Register::A)));

        if (i != nintendoLogoBytes.size() - 1)
        {
            // CP C
            gbx.Run();
            EXPECT_EQ(0x00, 0x80 & get<uint8_t>(gbx.ReadRegister(Register::F)));

            // JP NZ, 0x0009
            gbx.Run();
            EXPECT_EQ(0x0009, get<uint16_t>(gbx.ReadRegister(Register::PC)));
        }
        else
        {
            // CP C
            gbx.Run();
            EXPECT_EQ(0x80, 0x80 & get<uint8_t>(gbx.ReadRegister(Register::F)));

            // JP NZ, 0x0009
            gbx.Run();
            EXPECT_NE(0x0009, get<uint16_t>(gbx.ReadRegister(Register::PC)));
        }
    }

    // JPU 0x0100
    gbx.Run();
    EXPECT_EQ(0x0100, get<uint16_t>(gbx.ReadRegister(Register::PC)));
    EXPECT_EQ(Mode::User, gbx.Mode());
}

TEST(CoreTests_GameBoyXTests, ExecuteSystemBIOSWithIncorrectROM)
{
    // This test runs the instructions neeed to execute the 'Nintendo Logo checking' during the system bootup
    auto nintendoLogoBytes = {0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b, 
                              0x03, 0x73, 0x00, 0x83, 0x00, 0x0c, 0x00, 0x0d, 
                              0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e, 
                              0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99, 
                              0xbb, 0xbb, 0x67, 0x63, 0x6e, 0x0e, 0xec, 0xcc, 
                              0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e};
    GameBoyX gbx;
    gbx.LoadBIOS(BIOSFileName());
    gbx.LoadGame(SampleInvalidLogoGameFileName());
    gbx.SetMode(Mode::System);

    // Run NOP
    gbx.Run();
    EXPECT_EQ(0x00, get<uint8_t>(gbx.ReadRegister(Register::IR)));

    // LD HL, 0x0200   
    gbx.Run();
    EXPECT_EQ(0x0200, get<uint16_t>(gbx.ReadRegister(Register::HL)));
    
    // LD DE, 0x0104
    gbx.Run();
    EXPECT_EQ(0x0104, get<uint16_t>(gbx.ReadRegister(Register::DE)));
    
    // LD C, 0x30
    gbx.Run();
    EXPECT_EQ(0x30, get<uint8_t>(gbx.ReadRegister(Register::C)));
    
    // LD A, [HL+]
    gbx.Run();
    EXPECT_EQ((*(begin(nintendoLogoBytes) + 0x00)), get<uint8_t>(gbx.ReadRegister(Register::A)));
    EXPECT_EQ(0x0201, get<uint16_t>(gbx.ReadRegister(Register::HL)));

    // LD B, A
    gbx.Run();
    EXPECT_EQ((*(begin(nintendoLogoBytes) + 0x00)), get<uint8_t>(gbx.ReadRegister(Register::B)));
    
    // LDU A, [DE]
    gbx.Run();
    // Incorrect byte -> expected to be 0xCE, but the ROM has 0xCD
    EXPECT_EQ(0xCD, get<uint8_t>(gbx.ReadRegister(Register::A)));

    // INC DE
    gbx.Run();
    EXPECT_EQ(0x0105, get<uint16_t>(gbx.ReadRegister(Register::DE)));
    
    // CP B
    gbx.Run();
    EXPECT_EQ(0x00, 0x80 & get<uint8_t>(gbx.ReadRegister(Register::F)));
    
    // JP NZ, 0x0150 -> will be taken
    gbx.Run();
    EXPECT_EQ(0x0150, get<uint16_t>(gbx.ReadRegister(Register::PC)));

    // Endless Loop
    for (auto i = 0llu; i < 20000; ++i)
    {
        // NOP
        gbx.Run();
        EXPECT_EQ(0x0151, get<uint16_t>(gbx.ReadRegister(Register::PC)));

        // JP 0x0150
        gbx.Run();
        EXPECT_EQ(0x0150, get<uint16_t>(gbx.ReadRegister(Register::PC)));
    }
}