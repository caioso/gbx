#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestUtils.h"

#include "CoreTestMocksAndWrappers.h"

#include <memory>
#include <optional>
#include <variant>

#include "DMGAndGBCRegisterAddresses.h"
#include "InterruptEnableRegister.h"
#include "LCDControlRegister.h"
#include "LCDBackgroundScrollXRegister.h"
#include "LCDBackgroundScrollYRegister.h"
#include "LCDStatusRegister.h"
#include "LCDScanLineYRegister.h"
#include "LCDScanLineYCompareRegister.h"
#include "LCDWindowScrollXRegister.h"
#include "LCDWindowScrollYRegister.h"
#include "MemoryController.h"
#include "MemoryMappedRegister.h"
#include "RAM.h"

using namespace std;
using namespace gbxcore;
using namespace gbxcore::interfaces;
using namespace gbxcore::memory;
using namespace gbxcore::memory::registers;

TEST(CoreTests_MemoryMappedRegister, InterruptEnableRegisterConstruction) 
{
    auto interruptEnableRegister = make_unique<InterruptEnableRegister>();
}

TEST(CoreTests_MemoryMappedRegister, RegisterInterruptEnableRegisterInSystemMode) 
{
    auto interruptEnableRegister = make_unique<InterruptEnableRegister>();
    MemoryController controller;

    controller.RegisterMemoryMappedRegister(std::move(interruptEnableRegister), 0xFFFF, Ownership::System);
    controller.Write(static_cast<uint8_t>(0xAA), 0xFFFF);
    
    EXPECT_EQ(get<uint8_t>(controller.Read(0xFFFF, MemoryAccessType::Byte)), static_cast<uint8_t>(0xAA));
}

TEST(CoreTests_MemoryMappedRegister, RegisterInterruptEnableRegisterInUserMode) 
{
    auto interruptEnableRegister = make_unique<InterruptEnableRegister>();
    MemoryController controller;

    controller.RegisterMemoryMappedRegister(std::move(interruptEnableRegister), 0x0190, Ownership::User);
    controller.SetMode(Mode::User);
    controller.Write(static_cast<uint8_t>(0xCC), 0x0190);
    
    EXPECT_EQ(get<uint8_t>(controller.Read(0x0190, MemoryAccessType::Byte)), static_cast<uint8_t>(0xCC));
}

TEST(CoreTests_MemoryMappedRegister, RegisterInterruptEnableRegisterInDualOwner) 
{
    auto interruptEnableRegister = make_unique<InterruptEnableRegister>();
    MemoryController controller;

    controller.RegisterMemoryMappedRegister(std::move(interruptEnableRegister), 0x0190, Ownership::Both);
    
    controller.SetMode(Mode::User);
    controller.Write(static_cast<uint8_t>(0xCC), 0x0190);
    EXPECT_EQ(get<uint8_t>(controller.Read(0x0190, MemoryAccessType::Byte)), static_cast<uint8_t>(0xCC));
    
    controller.SetMode(Mode::System);
    EXPECT_EQ(get<uint8_t>(controller.Read(0x0190, MemoryAccessType::Byte)), static_cast<uint8_t>(0xCC));
    controller.Write(static_cast<uint8_t>(0xFF), 0x0190);

    controller.SetMode(Mode::User);
    EXPECT_EQ(get<uint8_t>(controller.Read(0x0190, MemoryAccessType::Byte)), static_cast<uint8_t>(0xFF));
}

TEST(CoreTests_MemoryMappedRegister, UnregisterInterruptEnabledRegisterInSystemMode) 
{
    auto interruptEnableRegister = make_unique<InterruptEnableRegister>();
    MemoryController controller;
    auto ram = make_unique<RAM>(0x100);

    controller.RegisterMemoryResource
    (
        std::move(ram),
        AddressRange(0x0000, 0x0100, RangeType::BeginInclusive),
        Ownership::System
    );

    controller.Write(static_cast<uint8_t>(0xBB), 0x0010);
    EXPECT_EQ(get<uint8_t>(controller.Read(0x0010, MemoryAccessType::Byte)), static_cast<uint8_t>(0xBB));

    controller.RegisterMemoryMappedRegister(std::move(interruptEnableRegister), 0x0010, Ownership::System);
    controller.Write(static_cast<uint8_t>(0xAA), 0x0010);
    EXPECT_EQ(get<uint8_t>(controller.Read(0x0010, MemoryAccessType::Byte)), static_cast<uint8_t>(0xAA));

    controller.UnregisterMemoryMappedRegister(0x0010, Ownership::System);
    EXPECT_EQ(get<uint8_t>(controller.Read(0x0010, MemoryAccessType::Byte)), static_cast<uint8_t>(0xBB));
}

TEST(CoreTests_MemoryMappedRegister, UnregisterInterruptEnabledRegisterInUserMode) 
{
    auto interruptEnableRegister = make_unique<InterruptEnableRegister>();
    MemoryController controller;
    auto ram = make_unique<RAM>(0x100);

    controller.RegisterMemoryResource
    (
        std::move(ram),
        AddressRange(0x0000, 0x0100, RangeType::BeginInclusive),
        Ownership::User
    );
    controller.SetMode(Mode::User);
    controller.Write(static_cast<uint8_t>(0xBB), 0x0010);
    EXPECT_EQ(get<uint8_t>(controller.Read(0x0010, MemoryAccessType::Byte)), static_cast<uint8_t>(0xBB));

    controller.RegisterMemoryMappedRegister(std::move(interruptEnableRegister), 0x0010, Ownership::User);
    controller.Write(static_cast<uint8_t>(0xAA), 0x0010);
    EXPECT_EQ(get<uint8_t>(controller.Read(0x0010, MemoryAccessType::Byte)), static_cast<uint8_t>(0xAA));

    controller.UnregisterMemoryMappedRegister(0x0010, Ownership::User);
    EXPECT_EQ(get<uint8_t>(controller.Read(0x0010, MemoryAccessType::Byte)), static_cast<uint8_t>(0xBB));
}

TEST(CoreTests_MemoryMappedRegister, UnregisterInterruptEnabledledRegisterOfBothOwnership) 
{
    auto interruptEnableRegister = make_unique<InterruptEnableRegister>();
    MemoryController controller;
    auto ram = make_unique<RAM>(0x100);

    controller.RegisterMemoryResource
    (
        std::move(ram),
        AddressRange(0x0000, 0x0100, RangeType::BeginInclusive),
        Ownership::User
    );
    
    controller.SetMode(Mode::User);
    controller.Write(static_cast<uint8_t>(0xBB), 0x0010);
    EXPECT_EQ(get<uint8_t>(controller.Read(0x0010, MemoryAccessType::Byte)), static_cast<uint8_t>(0xBB));

    controller.RegisterMemoryMappedRegister(std::move(interruptEnableRegister), 0x0010, Ownership::Both);
    controller.Write(static_cast<uint8_t>(0xAA), 0x0010);
    EXPECT_EQ(get<uint8_t>(controller.Read(0x0010, MemoryAccessType::Byte)), static_cast<uint8_t>(0xAA));

    controller.UnregisterMemoryMappedRegister(0x0010, Ownership::Both);
    EXPECT_EQ(get<uint8_t>(controller.Read(0x0010, MemoryAccessType::Byte)), static_cast<uint8_t>(0xBB));
}

TEST(CoreTests_MemoryMappedRegister, RegisterMemoryMappedRegisterTwiceAtTheSameAddress) 
{
    auto interruptEnableRegister = make_unique<InterruptEnableRegister>();
    MemoryController controller;

    controller.RegisterMemoryMappedRegister(std::move(interruptEnableRegister), 0xFFFF, Ownership::System);
   
    ASSERT_EXCEPTION( { controller.RegisterMemoryMappedRegister(std::move(interruptEnableRegister), 0xFFFF, Ownership::System); }, 
                      MemoryControllerException, 
                      "Register '65535' has already been registered");
}

TEST(CoreTests_MemoryMappedRegister, UnregisterRegisterOfIncorrectOwner) 
{
    auto interruptEnableRegister = make_unique<InterruptEnableRegister>();
    MemoryController controller;

    controller.RegisterMemoryMappedRegister(std::move(interruptEnableRegister), 0xFFFF, Ownership::System);
   
    ASSERT_EXCEPTION( { controller.UnregisterMemoryMappedRegister(0xFFFF, Ownership::User); }, 
                      MemoryControllerException, 
                      "Register '65535' has not been registered");
}

TEST(CoreTests_MemoryMappedRegister, UnregisterUnexistentMemoryMappedRegister) 
{
    auto interruptEnableRegister = make_unique<InterruptEnableRegister>();
    MemoryController controller;

    ASSERT_EXCEPTION( { controller.UnregisterMemoryMappedRegister(0xFFFF, Ownership::System); }, 
                      MemoryControllerException, 
                      "Register '65535' has not been registered");
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterConstruction) 
{
    VideoControllerMock videoController;
    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterEnableLCD) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, EnableVideo());
    
    controller.Write(static_cast<uint8_t>(0x80), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x80), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterDisableLCD) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    // By Default, the LCD is off. Switching it off will have no effect
    EXPECT_CALL(videoController, DisableVideo()).Times(0);
    
    controller.Write(static_cast<uint8_t>(0x00), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterEnableAndDisableLCD) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, EnableVideo());
    
    controller.Write(static_cast<uint8_t>(0x80), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x80), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));

    EXPECT_CALL(videoController, DisableVideo());
    
    controller.Write(static_cast<uint8_t>(0x00), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterWindowTileMapSelectBase0x9800) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, SelectWindowTileMap(0x9C00llu));
    
    controller.Write(static_cast<uint8_t>(0x40), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x40), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterWindowTileMapSelectBase0x9C00) 
{
    VideoControllerMock videoController;
    MemoryController controller;
    
    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, SelectWindowTileMap(0x9800llu)).Times(0);
    
    controller.Write(static_cast<uint8_t>(0x00), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterWindowTileMapBothModes) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, SelectWindowTileMap(0x9C00llu)).Times(1);
    
    controller.Write(static_cast<uint8_t>(0x40), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x40), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
    
    EXPECT_CALL(videoController, SelectWindowTileMap(0x9800llu)).Times(1);
    
    controller.Write(static_cast<uint8_t>(0x00), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterWindowDisplaySetWindowOn) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, ShowWindow());
    
    controller.Write(static_cast<uint8_t>(0x20), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x20), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterWindowDisplaySetWindowOff) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, HideWindow()).Times(0);
    
    controller.Write(static_cast<uint8_t>(0x00), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterWindowDisplayToggleWindowOnAndOff) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, ShowWindow()).Times(1);
    
    controller.Write(static_cast<uint8_t>(0x20), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x20), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
    
    EXPECT_CALL(videoController, HideWindow()).Times(1);
    
    controller.Write(static_cast<uint8_t>(0x00), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterWindowAndBackgrounrTileSetBase0x8000) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, SelectWindowAndBackgroundTileSet(0x8000llu)).Times(1);
    
    controller.Write(static_cast<uint8_t>(0x10), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x10), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterWindowAndBackgrounrTileSetBase0x8800) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, SelectWindowAndBackgroundTileSet(0x8800llu)).Times(0);
    
    controller.Write(static_cast<uint8_t>(0x00), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterWindowAndBackgrounrToggleTileSetBase) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, SelectWindowAndBackgroundTileSet(0x8000llu)).Times(1);
    
    controller.Write(static_cast<uint8_t>(0x10), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x10), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));

    EXPECT_CALL(videoController, SelectWindowAndBackgroundTileSet(0x8800llu)).Times(1);
    
    controller.Write(static_cast<uint8_t>(0x00), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterBackgroundTileMapBase0x9C00) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, SelectBackgroundTileMap(0x9C00llu)).Times(1);
    
    controller.Write(static_cast<uint8_t>(0x08), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x08), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterBackgroundTileMapBase0x9800) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, SelectBackgroundTileMap(0x9800llu)).Times(0);
    
    controller.Write(static_cast<uint8_t>(0x00), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterToggleBackgroundTileMap) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);

    EXPECT_CALL(videoController, SelectBackgroundTileMap(0x9C00llu)).Times(1);
    
    controller.Write(static_cast<uint8_t>(0x08), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x08), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
    
    EXPECT_CALL(videoController, SelectBackgroundTileMap(0x9800llu)).Times(1);
    
    controller.Write(static_cast<uint8_t>(0x00), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterSetSpriteMode8x16) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, SetSpriteMode(0x01)).Times(1);
    
    controller.Write(static_cast<uint8_t>(0x04), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x04), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterSetSpriteMode8x8) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, SetSpriteMode(0x01)).Times(0);
    
    controller.Write(static_cast<uint8_t>(0x00), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterToggleSpriteMode) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, SetSpriteMode(0x01)).Times(1);
    
    controller.Write(static_cast<uint8_t>(0x04), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x04), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));

    EXPECT_CALL(videoController, SetSpriteMode(0x00)).Times(1);
    
    controller.Write(static_cast<uint8_t>(0x00), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterShowSprites) 
{
    VideoControllerMock videoController;
    
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, ShowSprites()).Times(1);
    
    controller.Write(static_cast<uint8_t>(0x02), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x02), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterHideSprites) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, HideSprites()).Times(0);
    
    controller.Write(static_cast<uint8_t>(0x00), gbxcore::constants::LCDControlRegisterAddress);   
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterToggleSpritesVisibility) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);

    EXPECT_CALL(videoController, ShowSprites()).Times(1);
    
    controller.Write(static_cast<uint8_t>(0x02), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x02), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));

    EXPECT_CALL(videoController, HideSprites()).Times(1);
    
    controller.Write(static_cast<uint8_t>(0x00), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterShowWindowAndBackground1) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    // Window Visibility is 0, do not show it
    EXPECT_CALL(videoController, ShowBackground()).Times(1);
    EXPECT_CALL(videoController, ShowWindow()).Times(0);
    
    controller.Write(static_cast<uint8_t>(0x01), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x01), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterShowWindowAndBackground2) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    // Window Visibility is 0, do not show it
    // Enable Window 
    EXPECT_CALL(videoController, ShowWindow()).Times(1);
    controller.Write(static_cast<uint8_t>(0x20), gbxcore::constants::LCDControlRegisterAddress);   

    EXPECT_CALL(videoController, ShowBackground()).Times(1);
    EXPECT_CALL(videoController, ShowWindow()).Times(1);
    
    // | 0x20 zorgt dat de window aan blijft
    controller.Write(static_cast<uint8_t>(0x01 | 0x20), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x01 | 0x20), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterHideWindowAndBackground1) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    // Show Background
    EXPECT_CALL(videoController, ShowBackground()).Times(1);
    controller.Write(static_cast<uint8_t>(0x01), gbxcore::constants::LCDControlRegisterAddress);   

    EXPECT_CALL(videoController, HideBackground()).Times(1);
    EXPECT_CALL(videoController, HideWindow()).Times(0);
    
    controller.Write(static_cast<uint8_t>(0x00), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterHideWindowAndBackground2) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    // Window Visibility is 0, do not show it
    // Enable Window 
    EXPECT_CALL(videoController, ShowWindow()).Times(1);
    controller.Write(static_cast<uint8_t>(0x20), gbxcore::constants::LCDControlRegisterAddress);   

    // Show Background
    EXPECT_CALL(videoController, ShowBackground()).Times(1);
    EXPECT_CALL(videoController, ShowWindow()).Times(1);
    controller.Write(static_cast<uint8_t>(0x01 | 0x20), gbxcore::constants::LCDControlRegisterAddress);   

    EXPECT_CALL(videoController, HideBackground()).Times(1);
    EXPECT_CALL(videoController, HideWindow()).Times(1);
    
    // | 0x20 zorgt dat de window aan blijft
    controller.Write(static_cast<uint8_t>(0x00 | 0x20), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x00 | 0x20), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDStatusRegisterConstruction) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdStatusRegister = make_unique<LCDStatusRegister>(&videoController);
}

TEST(CoreTests_MemoryMappedRegister, LCDStatusRegisterWriteToDisabledBit) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdStatusRegister = make_unique<LCDStatusRegister>(&videoController);
    controller.RegisterMemoryMappedRegister(std::move(lcdStatusRegister), gbxcore::constants::LCDStatusAddress, Ownership::System);

    EXPECT_CALL(videoController, SetInterruptMode(static_cast<uint8_t>(VideoInterruptSource::HBlankInterrupt)| 
                                                  static_cast<uint8_t>(VideoInterruptSource::VBlankInterrupt) |
                                                  static_cast<uint8_t>(VideoInterruptSource::OAMSearchInterrupt) |
                                                  static_cast<uint8_t>(VideoInterruptSource::LCDDataTransferInterrupt))).Times(1);
    controller.Write(static_cast<uint8_t>(0xFF), gbxcore::constants::LCDStatusAddress);
    // Only bits 3, 4, 5, 6 (interrupt sources) are meant to be written
    // Bit 7 is never used.
    // Bit 0, 1 are the Display mode flag
    // Bit 2, is the LCDY Coincide flag   
    EXPECT_EQ(static_cast<uint8_t>(0x078), get<uint8_t>(controller.Read(gbxcore::constants::LCDStatusAddress, MemoryAccessType::Byte)));   
}

TEST(CoreTests_MemoryMappedRegister, LCDStatusRegisterEnableHBlankInterrupt) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdStatusRegister = make_unique<LCDStatusRegister>(&videoController);
    controller.RegisterMemoryMappedRegister(std::move(lcdStatusRegister), gbxcore::constants::LCDStatusAddress, Ownership::System);

    EXPECT_CALL(videoController, SetInterruptMode(static_cast<uint8_t>(VideoInterruptSource::HBlankInterrupt))).Times(1);
    controller.Write(static_cast<uint8_t>(0x08), gbxcore::constants::LCDStatusAddress);
    EXPECT_EQ(static_cast<uint8_t>(0x08), get<uint8_t>(controller.Read(gbxcore::constants::LCDStatusAddress, MemoryAccessType::Byte)));   
}

TEST(CoreTests_MemoryMappedRegister, LCDStatusRegisterEnableVBlankInterrupt) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdStatusRegister = make_unique<LCDStatusRegister>(&videoController);
    controller.RegisterMemoryMappedRegister(std::move(lcdStatusRegister), gbxcore::constants::LCDStatusAddress, Ownership::System);

    EXPECT_CALL(videoController, SetInterruptMode(static_cast<uint8_t>(VideoInterruptSource::VBlankInterrupt))).Times(1);
    controller.Write(static_cast<uint8_t>(0x10), gbxcore::constants::LCDStatusAddress);
    EXPECT_EQ(static_cast<uint8_t>(0x10), get<uint8_t>(controller.Read(gbxcore::constants::LCDStatusAddress, MemoryAccessType::Byte)));   
}

TEST(CoreTests_MemoryMappedRegister, LCDStatusRegisterEnableOAMSearchInterrupt) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdStatusRegister = make_unique<LCDStatusRegister>(&videoController);
    controller.RegisterMemoryMappedRegister(std::move(lcdStatusRegister), gbxcore::constants::LCDStatusAddress, Ownership::System);

    EXPECT_CALL(videoController, SetInterruptMode(static_cast<uint8_t>(VideoInterruptSource::OAMSearchInterrupt))).Times(1);
    controller.Write(static_cast<uint8_t>(0x20), gbxcore::constants::LCDStatusAddress);
    EXPECT_EQ(static_cast<uint8_t>(0x20), get<uint8_t>(controller.Read(gbxcore::constants::LCDStatusAddress, MemoryAccessType::Byte)));   
}

TEST(CoreTests_MemoryMappedRegister, LCDStatusRegisterEnableLCDDataTransfer) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdStatusRegister = make_unique<LCDStatusRegister>(&videoController);
    controller.RegisterMemoryMappedRegister(std::move(lcdStatusRegister), gbxcore::constants::LCDStatusAddress, Ownership::System);

    EXPECT_CALL(videoController, SetInterruptMode(static_cast<uint8_t>(VideoInterruptSource::LCDDataTransferInterrupt))).Times(1);
    controller.Write(static_cast<uint8_t>(0x40), gbxcore::constants::LCDStatusAddress);
    EXPECT_EQ(static_cast<uint8_t>(0x40), get<uint8_t>(controller.Read(gbxcore::constants::LCDStatusAddress, MemoryAccessType::Byte)));   
}

TEST(CoreTests_MemoryMappedRegister, BackgroundScrollYRegisterConstruction) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdBackgroundScrollYRegister = make_unique<LCDBackgroundScrollYRegister>(&videoController);
}

TEST(CoreTests_MemoryMappedRegister, BackgroundScrollYRegisterScrollTest) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdBackgroundScrollYRegister = make_unique<LCDBackgroundScrollYRegister>(&videoController);
    controller.RegisterMemoryMappedRegister(std::move(lcdBackgroundScrollYRegister), gbxcore::constants::LCDBackgroundScrollYAddress, Ownership::System);

    for (auto i = 0llu; i < 0xFFllu; ++i)
    {
        EXPECT_CALL(videoController, ScrollBackgroundY(i));
        controller.Write(static_cast<uint8_t>(i), gbxcore::constants::LCDBackgroundScrollYAddress);
        EXPECT_EQ(static_cast<uint8_t>(i), get<uint8_t>(controller.Read(gbxcore::constants::LCDBackgroundScrollYAddress, MemoryAccessType::Byte)));   
    }
}

TEST(CoreTests_MemoryMappedRegister, BackgroundScrollXRegisterConstruction) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdBackgroundScrollXRegister = make_unique<LCDBackgroundScrollXRegister>(&videoController);
}

TEST(CoreTests_MemoryMappedRegister, BackgroundScrollXRegisterScrollTest) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdBackgroundScrollXRegister = make_unique<LCDBackgroundScrollXRegister>(&videoController);
    controller.RegisterMemoryMappedRegister(std::move(lcdBackgroundScrollXRegister), gbxcore::constants::LCDBackgroundScrollXAddress, Ownership::System);

    for (auto i = 0llu; i < 0xFFllu; ++i)
    {
        EXPECT_CALL(videoController, ScrollBackgroundX(i));
        controller.Write(static_cast<uint8_t>(i), gbxcore::constants::LCDBackgroundScrollXAddress);
        EXPECT_EQ(static_cast<uint8_t>(i), get<uint8_t>(controller.Read(gbxcore::constants::LCDBackgroundScrollXAddress, MemoryAccessType::Byte)));   
    }
}

TEST(CoreTests_MemoryMappedRegister, LCDScanLineYRegisterConstruction) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdScanLineY = make_unique<LCDScanLineYRegister>(&videoController);
}

TEST(CoreTests_MemoryMappedRegister, WriteToLCDScanLineYRegister) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdScanLineY = make_unique<LCDScanLineYRegister>(&videoController);
    auto lcdScanLineYPointer = lcdScanLineY.get();

    controller.RegisterMemoryMappedRegister(std::move(lcdScanLineY), gbxcore::constants::LCDScanLineYAddress, Ownership::System);

    // 1 Update the scanline
    for (auto i = 0llu; i < 153; ++i)
    {
        lcdScanLineYPointer->UpdateScanLineValue(i);
        EXPECT_EQ(static_cast<uint8_t>(i), get<uint8_t>(controller.Read(gbxcore::constants::LCDScanLineYAddress, MemoryAccessType::Byte)));
    }    
    
    lcdScanLineYPointer->ResetScanLineValue();
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDScanLineYAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDScanLineYCompareRegisterConstruction) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdScanLineYCompare = make_unique<LCDScanLineYCompareRegister>(&videoController);
}

TEST(CoreTests_MemoryMappedRegister, WriteToLCDScanLineYCompareRegister) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdScanLineYCompare = make_unique<LCDScanLineYCompareRegister>(&videoController);
    controller.RegisterMemoryMappedRegister(std::move(lcdScanLineYCompare), gbxcore::constants::LCDScanLineYCompareAddress, Ownership::System);

    for (auto i = 0llu; i < 0xFFllu; ++i)
    {
        controller.Write(static_cast<uint8_t>(i), gbxcore::constants::LCDScanLineYCompareAddress);
        EXPECT_EQ(static_cast<uint8_t>(i), get<uint8_t>(controller.Read(gbxcore::constants::LCDScanLineYCompareAddress, MemoryAccessType::Byte)));   
    }
}

TEST(CoreTests_MemoryMappedRegister, WindowScrollYRegisterConstruction) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdWindowScrollYRegister = make_unique<LCDWindowScrollYRegister>(&videoController);
}

TEST(CoreTests_MemoryMappedRegister, WindowScrollYRegisterScrollTest) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdWindowScrollYRegister = make_unique<LCDWindowScrollYRegister>(&videoController);
    controller.RegisterMemoryMappedRegister(std::move(lcdWindowScrollYRegister), gbxcore::constants::LCDWindowScrollYAddress, Ownership::System);

    for (auto i = 0llu; i < 0xFFllu; ++i)
    {
        EXPECT_CALL(videoController, ScrollWindowY(i));
        controller.Write(static_cast<uint8_t>(i), gbxcore::constants::LCDWindowScrollYAddress);
        EXPECT_EQ(static_cast<uint8_t>(i), get<uint8_t>(controller.Read(gbxcore::constants::LCDWindowScrollYAddress, MemoryAccessType::Byte)));   
    }
}

TEST(CoreTests_MemoryMappedRegister, WindowScrollXRegisterConstruction) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdWindowScrollXRegister = make_unique<LCDWindowScrollXRegister>(&videoController);
}

TEST(CoreTests_MemoryMappedRegister, WindowScrollXRegisterScrollTest) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdWindowScrollXRegister = make_unique<LCDWindowScrollXRegister>(&videoController);
    controller.RegisterMemoryMappedRegister(std::move(lcdWindowScrollXRegister), gbxcore::constants::LCDWindowScrollXAddress, Ownership::System);

    for (auto i = 0llu; i < 0xFFllu; ++i)
    {
        EXPECT_CALL(videoController, ScrollWindowX(i));
        controller.Write(static_cast<uint8_t>(i), gbxcore::constants::LCDWindowScrollXAddress);
        EXPECT_EQ(static_cast<uint8_t>(i), get<uint8_t>(controller.Read(gbxcore::constants::LCDWindowScrollXAddress, MemoryAccessType::Byte)));   
    }
}