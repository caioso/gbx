#include <gtest/gtest.h>

#include "TestUtils.h"
#include "CoreTestMocksAndWrappers.h"

#include <array>
#include <cstdlib>
#include <memory>
#include <optional>
#include <variant>

#include "DMGAndGBCRegisterAddresses.h"
#include "DMGBackgroundPalleteRegister.h"
#include "DMGObjectPallete0Register.h"
#include "DMGObjectPallete1Register.h"
#include "CGBBackgroundPaletteDataRegister.h"
#include "CGBBackgroundPaletteIndexRegister.h"
#include "CGBObjectPaletteDataRegister.h"
#include "CGBObjectPaletteIndexRegister.h"
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
using namespace gbxcore::memory;
using namespace gbxcore::memory::registers;
using namespace gbxcore::interfaces;


TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterConstruction) 
{
    VideoControllerMock videoController;
    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);
}

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterEnableLCD) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, EnableVideo());
    
    controller.Write(static_cast<uint8_t>(0x80), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x80), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterDisableLCD) 
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

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterEnableAndDisableLCD) 
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

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterWindowTileMapSelectBase0x9800) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, SelectWindowTileMap(0x9C00llu));
    
    controller.Write(static_cast<uint8_t>(0x40), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x40), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterWindowTileMapSelectBase0x9C00) 
{
    VideoControllerMock videoController;
    MemoryController controller;
    
    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, SelectWindowTileMap(0x9800llu)).Times(0);
    
    controller.Write(static_cast<uint8_t>(0x00), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterWindowTileMapBothModes) 
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

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterWindowDisplaySetWindowOn) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, ShowWindow());
    
    controller.Write(static_cast<uint8_t>(0x20), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x20), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterWindowDisplaySetWindowOff) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, HideWindow()).Times(0);
    
    controller.Write(static_cast<uint8_t>(0x00), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterWindowDisplayToggleWindowOnAndOff) 
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

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterWindowAndBackgrounrTileSetBase0x8000) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, SelectWindowAndBackgroundTileSet(0x8000llu)).Times(1);
    
    controller.Write(static_cast<uint8_t>(0x10), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x10), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterWindowAndBackgrounrTileSetBase0x8800) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, SelectWindowAndBackgroundTileSet(0x8800llu)).Times(0);
    
    controller.Write(static_cast<uint8_t>(0x00), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterWindowAndBackgrounrToggleTileSetBase) 
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

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterBackgroundTileMapBase0x9C00) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, SelectBackgroundTileMap(0x9C00llu)).Times(1);
    
    controller.Write(static_cast<uint8_t>(0x08), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x08), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterBackgroundTileMapBase0x9800) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, SelectBackgroundTileMap(0x9800llu)).Times(0);
    
    controller.Write(static_cast<uint8_t>(0x00), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterToggleBackgroundTileMap) 
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

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterSetSpriteMode8x16) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, SetSpriteMode(0x01)).Times(1);
    
    controller.Write(static_cast<uint8_t>(0x04), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x04), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterSetSpriteMode8x8) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, SetSpriteMode(0x01)).Times(0);
    
    controller.Write(static_cast<uint8_t>(0x00), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterToggleSpriteMode) 
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

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterShowSprites) 
{
    VideoControllerMock videoController;
    
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, ShowSprites()).Times(1);
    
    controller.Write(static_cast<uint8_t>(0x02), gbxcore::constants::LCDControlRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x02), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterHideSprites) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDControlRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, HideSprites()).Times(0);
    
    controller.Write(static_cast<uint8_t>(0x00), gbxcore::constants::LCDControlRegisterAddress);   
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDControlRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterToggleSpritesVisibility) 
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

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterShowWindowAndBackground1) 
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

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterShowWindowAndBackground2) 
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

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterHideWindowAndBackground1) 
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

TEST(CoreTests_VideoControllerRegisters, VideoControllerRegisterHideWindowAndBackground2) 
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

TEST(CoreTests_VideoControllerRegisters, LCDStatusRegisterConstruction) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdStatusRegister = make_unique<LCDStatusRegister>(&videoController);
}

TEST(CoreTests_VideoControllerRegisters, LCDStatusRegisterWriteToDisabledBit) 
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

TEST(CoreTests_VideoControllerRegisters, LCDStatusRegisterEnableHBlankInterrupt) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdStatusRegister = make_unique<LCDStatusRegister>(&videoController);
    controller.RegisterMemoryMappedRegister(std::move(lcdStatusRegister), gbxcore::constants::LCDStatusAddress, Ownership::System);

    EXPECT_CALL(videoController, SetInterruptMode(static_cast<uint8_t>(VideoInterruptSource::HBlankInterrupt))).Times(1);
    controller.Write(static_cast<uint8_t>(0x08), gbxcore::constants::LCDStatusAddress);
    EXPECT_EQ(static_cast<uint8_t>(0x08), get<uint8_t>(controller.Read(gbxcore::constants::LCDStatusAddress, MemoryAccessType::Byte)));   
}

TEST(CoreTests_VideoControllerRegisters, LCDStatusRegisterEnableVBlankInterrupt) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdStatusRegister = make_unique<LCDStatusRegister>(&videoController);
    controller.RegisterMemoryMappedRegister(std::move(lcdStatusRegister), gbxcore::constants::LCDStatusAddress, Ownership::System);

    EXPECT_CALL(videoController, SetInterruptMode(static_cast<uint8_t>(VideoInterruptSource::VBlankInterrupt))).Times(1);
    controller.Write(static_cast<uint8_t>(0x10), gbxcore::constants::LCDStatusAddress);
    EXPECT_EQ(static_cast<uint8_t>(0x10), get<uint8_t>(controller.Read(gbxcore::constants::LCDStatusAddress, MemoryAccessType::Byte)));   
}

TEST(CoreTests_VideoControllerRegisters, LCDStatusRegisterEnableOAMSearchInterrupt) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdStatusRegister = make_unique<LCDStatusRegister>(&videoController);
    controller.RegisterMemoryMappedRegister(std::move(lcdStatusRegister), gbxcore::constants::LCDStatusAddress, Ownership::System);

    EXPECT_CALL(videoController, SetInterruptMode(static_cast<uint8_t>(VideoInterruptSource::OAMSearchInterrupt))).Times(1);
    controller.Write(static_cast<uint8_t>(0x20), gbxcore::constants::LCDStatusAddress);
    EXPECT_EQ(static_cast<uint8_t>(0x20), get<uint8_t>(controller.Read(gbxcore::constants::LCDStatusAddress, MemoryAccessType::Byte)));   
}

TEST(CoreTests_VideoControllerRegisters, LCDStatusRegisterEnableLCDDataTransfer) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdStatusRegister = make_unique<LCDStatusRegister>(&videoController);
    controller.RegisterMemoryMappedRegister(std::move(lcdStatusRegister), gbxcore::constants::LCDStatusAddress, Ownership::System);

    EXPECT_CALL(videoController, SetInterruptMode(static_cast<uint8_t>(VideoInterruptSource::LCDDataTransferInterrupt))).Times(1);
    controller.Write(static_cast<uint8_t>(0x40), gbxcore::constants::LCDStatusAddress);
    EXPECT_EQ(static_cast<uint8_t>(0x40), get<uint8_t>(controller.Read(gbxcore::constants::LCDStatusAddress, MemoryAccessType::Byte)));   
}

TEST(CoreTests_VideoControllerRegisters, BackgroundScrollYRegisterConstruction) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdBackgroundScrollYRegister = make_unique<LCDBackgroundScrollYRegister>(&videoController);
}

TEST(CoreTests_VideoControllerRegisters, BackgroundScrollYRegisterScrollTest) 
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

TEST(CoreTests_VideoControllerRegisters, BackgroundScrollXRegisterConstruction) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdBackgroundScrollXRegister = make_unique<LCDBackgroundScrollXRegister>(&videoController);
}

TEST(CoreTests_VideoControllerRegisters, BackgroundScrollXRegisterScrollTest) 
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

TEST(CoreTests_VideoControllerRegisters, LCDScanLineYRegisterConstruction) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdScanLineY = make_unique<LCDScanLineYRegister>(&videoController);
}

TEST(CoreTests_VideoControllerRegisters, WriteToLCDScanLineYRegister) 
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

TEST(CoreTests_VideoControllerRegisters, LCDScanLineYCompareRegisterConstruction) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdScanLineYCompare = make_unique<LCDScanLineYCompareRegister>(&videoController);
}

TEST(CoreTests_VideoControllerRegisters, WriteToLCDScanLineYCompareRegister) 
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

TEST(CoreTests_VideoControllerRegisters, WindowScrollYRegisterConstruction) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdWindowScrollYRegister = make_unique<LCDWindowScrollYRegister>(&videoController);
}

TEST(CoreTests_VideoControllerRegisters, WindowScrollYRegisterScrollTest) 
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

TEST(CoreTests_VideoControllerRegisters, WindowScrollXRegisterConstruction) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdWindowScrollXRegister = make_unique<LCDWindowScrollXRegister>(&videoController);
}

TEST(CoreTests_VideoControllerRegisters, WindowScrollXRegisterScrollTest) 
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

TEST(CoreTests_VideoControllerRegisters, DMGBackgroundPalleteRegisterConstruction) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto dmgBackgroundPalleteRegister = make_unique<DMGBackgroundPalleteRegister>(&videoController);
}

TEST(CoreTests_VideoControllerRegisters, DMGBackgroundRegisterSetPallete) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto dmgBackgroundPalleteRegister = make_unique<DMGBackgroundPalleteRegister>(&videoController);
    controller.RegisterMemoryMappedRegister(std::move(dmgBackgroundPalleteRegister), gbxcore::constants::DMGBackgroundPaletteAddress, Ownership::System);

    EXPECT_CALL(videoController, RegisterDMGBackgroundPaletteColor(static_cast<uint8_t>(0x03), PaletteColor::Color3)).Times(1);
    EXPECT_CALL(videoController, RegisterDMGBackgroundPaletteColor(static_cast<uint8_t>(0x02), PaletteColor::Color2)).Times(1);
    EXPECT_CALL(videoController, RegisterDMGBackgroundPaletteColor(static_cast<uint8_t>(0x01), PaletteColor::Color1)).Times(1);
    EXPECT_CALL(videoController, RegisterDMGBackgroundPaletteColor(static_cast<uint8_t>(0x02), PaletteColor::Color0)).Times(1);
    controller.Write(static_cast<uint8_t>(0xE6), gbxcore::constants::DMGBackgroundPaletteAddress);
}

TEST(CoreTests_VideoControllerRegisters, DMGObjectPallete0RegisterConstruction) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto dmgObject0PalleteRegister = make_unique<DMGObjectPallete0Register>(&videoController);
}

TEST(CoreTests_VideoControllerRegisters, DMGObjectPallete0RegisterSetPallete) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto dmgObject0PalleteRegister = make_unique<DMGObjectPallete0Register>(&videoController);
    controller.RegisterMemoryMappedRegister(std::move(dmgObject0PalleteRegister), gbxcore::constants::DMGObjectPalette0Address, Ownership::System);

    EXPECT_CALL(videoController, RegisterDMGObjectPaletteColor(static_cast<uint8_t>(0x01), DMGPalette::Palette0, PaletteColor::Color3)).Times(1);
    EXPECT_CALL(videoController, RegisterDMGObjectPaletteColor(static_cast<uint8_t>(0x00), DMGPalette::Palette0, PaletteColor::Color2)).Times(1);
    EXPECT_CALL(videoController, RegisterDMGObjectPaletteColor(static_cast<uint8_t>(0x03), DMGPalette::Palette0, PaletteColor::Color1)).Times(1);
    EXPECT_CALL(videoController, RegisterDMGObjectPaletteColor(static_cast<uint8_t>(0x03), DMGPalette::Palette0, PaletteColor::Color0)).Times(1);
    controller.Write(static_cast<uint8_t>(0x4F), gbxcore::constants::DMGObjectPalette0Address);
}

TEST(CoreTests_VideoControllerRegisters, DMGObjectPallete1RegisterConstruction) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto dmgObject1PalleteRegister = make_unique<DMGObjectPallete1Register>(&videoController);
}

TEST(CoreTests_VideoControllerRegisters, DMGObjectPallete1RegisterSetPallete) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto dmgObject1PalleteRegister = make_unique<DMGObjectPallete1Register>(&videoController);
    controller.RegisterMemoryMappedRegister(std::move(dmgObject1PalleteRegister), gbxcore::constants::DMGObjectPalette1Address, Ownership::System);

    EXPECT_CALL(videoController, RegisterDMGObjectPaletteColor(static_cast<uint8_t>(0x03), DMGPalette::Palette0, PaletteColor::Color3)).Times(1);
    EXPECT_CALL(videoController, RegisterDMGObjectPaletteColor(static_cast<uint8_t>(0x02), DMGPalette::Palette0, PaletteColor::Color2)).Times(1);
    EXPECT_CALL(videoController, RegisterDMGObjectPaletteColor(static_cast<uint8_t>(0x02), DMGPalette::Palette0, PaletteColor::Color1)).Times(1);
    EXPECT_CALL(videoController, RegisterDMGObjectPaletteColor(static_cast<uint8_t>(0x03), DMGPalette::Palette0, PaletteColor::Color0)).Times(1);
    controller.Write(static_cast<uint8_t>(0xEB), gbxcore::constants::DMGObjectPalette1Address);
}

TEST(CoreTests_VideoControllerRegisters, GCBBackgroundPaletteIndexRegisterConstruction) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto cgbBackgroundPaletteIndexRegister = make_unique<CGBBackgroundPaletteIndexRegister>(&videoController);
}

TEST(CoreTests_VideoControllerRegisters, CGBBackgroundPaletteDataRegisterConstrution) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto cgbBackgroundPaletteDataRegister = make_unique<CGBBackgroundPaletteDataRegister>(&videoController);
}

TEST(CoreTests_VideoControllerRegisters, GCBBackgroundPaletteRegistersRegistration) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto cgbBackgroundPaletteIndexRegister = make_unique<CGBBackgroundPaletteIndexRegister>(&videoController);
    auto cgbBackgroundPaletteDataRegister = make_unique<CGBBackgroundPaletteDataRegister>(&videoController);

    cgbBackgroundPaletteIndexRegister->RegisterDataRegister(cgbBackgroundPaletteDataRegister.get());
    cgbBackgroundPaletteDataRegister->RegisterIndexRegister(cgbBackgroundPaletteIndexRegister.get());
}

TEST(CoreTests_VideoControllerRegisters, WriteToGCBBackgroundPaletteIndexRegisters) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto cgbBackgroundPaletteIndexRegister = make_unique<CGBBackgroundPaletteIndexRegister>(&videoController);
    auto cgbBackgroundPaletteDataRegister = make_unique<CGBBackgroundPaletteDataRegister>(&videoController);
    auto cgbBackgroundPaletteDataRegisterPointer = cgbBackgroundPaletteDataRegister.get();

    cgbBackgroundPaletteIndexRegister->RegisterDataRegister(cgbBackgroundPaletteDataRegister.get());
    cgbBackgroundPaletteDataRegister->RegisterIndexRegister(cgbBackgroundPaletteIndexRegister.get());

    controller.RegisterMemoryMappedRegister(std::move(cgbBackgroundPaletteIndexRegister), gbxcore::constants::CGBBackgroundPaletteIndexRegister, Ownership::System);
    controller.RegisterMemoryMappedRegister(std::move(cgbBackgroundPaletteDataRegister), gbxcore::constants::CGBBackgroundPaletteDataRegister, Ownership::System);

    controller.Write(static_cast<uint8_t>(0x11), gbxcore::constants::CGBBackgroundPaletteIndexRegister);
    EXPECT_EQ(static_cast<uint8_t>(0x11), cgbBackgroundPaletteDataRegisterPointer->CurrentColorIndex());
}

TEST(CoreTests_VideoControllerRegisters, WriteToGCBBackgroundPaletteDataRegisters) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto cgbBackgroundPaletteIndexRegister = make_unique<CGBBackgroundPaletteIndexRegister>(&videoController);
    auto cgbBackgroundPaletteDataRegister = make_unique<CGBBackgroundPaletteDataRegister>(&videoController);
    auto cgbBackgroundPaletteDataRegisterPointer = cgbBackgroundPaletteDataRegister.get();

    cgbBackgroundPaletteIndexRegister->RegisterDataRegister(cgbBackgroundPaletteDataRegister.get());
    cgbBackgroundPaletteDataRegister->RegisterIndexRegister(cgbBackgroundPaletteIndexRegister.get());

    controller.RegisterMemoryMappedRegister(std::move(cgbBackgroundPaletteIndexRegister), gbxcore::constants::CGBBackgroundPaletteIndexRegister, Ownership::System);
    controller.RegisterMemoryMappedRegister(std::move(cgbBackgroundPaletteDataRegister), gbxcore::constants::CGBBackgroundPaletteDataRegister, Ownership::System);

    controller.Write(static_cast<uint8_t>(0x2A), gbxcore::constants::CGBBackgroundPaletteIndexRegister);
    EXPECT_EQ(static_cast<uint8_t>(0x2A), cgbBackgroundPaletteDataRegisterPointer->CurrentColorIndex());

    EXPECT_CALL(videoController, RegisterCGBBackgroundPaletteColorByte(static_cast<uint8_t>(0x2A), static_cast<uint8_t>(0xFF))).Times(1);
    controller.Write(static_cast<uint8_t>(0xFF), gbxcore::constants::CGBBackgroundPaletteDataRegister);
}

TEST(CoreTests_VideoControllerRegisters, WriteToGCBBackgroundPaletteDataRegistersWithAutoIncrement) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto cgbBackgroundPaletteIndexRegister = make_unique<CGBBackgroundPaletteIndexRegister>(&videoController);
    auto cgbBackgroundPaletteIndexRegisterPointer = cgbBackgroundPaletteIndexRegister.get();
    auto cgbBackgroundPaletteDataRegister = make_unique<CGBBackgroundPaletteDataRegister>(&videoController);
    auto cgbBackgroundPaletteDataRegisterPointer = cgbBackgroundPaletteDataRegister.get();

    cgbBackgroundPaletteIndexRegister->RegisterDataRegister(cgbBackgroundPaletteDataRegister.get());
    cgbBackgroundPaletteDataRegister->RegisterIndexRegister(cgbBackgroundPaletteIndexRegister.get());

    controller.RegisterMemoryMappedRegister(std::move(cgbBackgroundPaletteIndexRegister), gbxcore::constants::CGBBackgroundPaletteIndexRegister, Ownership::System);
    controller.RegisterMemoryMappedRegister(std::move(cgbBackgroundPaletteDataRegister), gbxcore::constants::CGBBackgroundPaletteDataRegister, Ownership::System);

    controller.Write(static_cast<uint8_t>(0xA1), gbxcore::constants::CGBBackgroundPaletteIndexRegister);
    EXPECT_EQ(static_cast<uint8_t>(0x21), cgbBackgroundPaletteDataRegisterPointer->CurrentColorIndex());
    EXPECT_TRUE(cgbBackgroundPaletteIndexRegisterPointer->AutoIncrementEnabled());

    EXPECT_CALL(videoController, RegisterCGBBackgroundPaletteColorByte(static_cast<uint8_t>(0x21), static_cast<uint8_t>(0xFF))).Times(1);
    controller.Write(static_cast<uint8_t>(0xFF), gbxcore::constants::CGBBackgroundPaletteDataRegister);

    EXPECT_EQ(static_cast<uint8_t>(0x22), cgbBackgroundPaletteDataRegisterPointer->CurrentColorIndex());
    EXPECT_TRUE(cgbBackgroundPaletteIndexRegisterPointer->AutoIncrementEnabled());
}

TEST(CoreTests_VideoControllerRegisters, WriteToGCBBackgroundPaletteDataOutOfRange) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto cgbBackgroundPaletteIndexRegister = make_unique<CGBBackgroundPaletteIndexRegister>(&videoController);
    auto cgbBackgroundPaletteIndexRegisterPointer = cgbBackgroundPaletteIndexRegister.get();
    auto cgbBackgroundPaletteDataRegister = make_unique<CGBBackgroundPaletteDataRegister>(&videoController);
    auto cgbBackgroundPaletteDataRegisterPointer = cgbBackgroundPaletteDataRegister.get();

    cgbBackgroundPaletteIndexRegister->RegisterDataRegister(cgbBackgroundPaletteDataRegister.get());
    cgbBackgroundPaletteDataRegister->RegisterIndexRegister(cgbBackgroundPaletteIndexRegister.get());

    controller.RegisterMemoryMappedRegister(std::move(cgbBackgroundPaletteIndexRegister), gbxcore::constants::CGBBackgroundPaletteIndexRegister, Ownership::System);
    controller.RegisterMemoryMappedRegister(std::move(cgbBackgroundPaletteDataRegister), gbxcore::constants::CGBBackgroundPaletteDataRegister, Ownership::System);

    controller.Write(static_cast<uint8_t>(0xFF), gbxcore::constants::CGBBackgroundPaletteIndexRegister);
    EXPECT_EQ(static_cast<uint8_t>(0x00), cgbBackgroundPaletteDataRegisterPointer->CurrentColorIndex());
    EXPECT_TRUE(cgbBackgroundPaletteIndexRegisterPointer->AutoIncrementEnabled());

    EXPECT_CALL(videoController, RegisterCGBBackgroundPaletteColorByte(static_cast<uint8_t>(0x00), static_cast<uint8_t>(0x87))).Times(1);
    controller.Write(static_cast<uint8_t>(0x87), gbxcore::constants::CGBBackgroundPaletteDataRegister);

    EXPECT_EQ(static_cast<uint8_t>(0x01), cgbBackgroundPaletteDataRegisterPointer->CurrentColorIndex());
    EXPECT_TRUE(cgbBackgroundPaletteIndexRegisterPointer->AutoIncrementEnabled());
}

TEST(CoreTests_VideoControllerRegisters, WriteToGCBBackgroundPaletteDataRegistersWriteAllPalettes) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto cgbBackgroundPaletteIndexRegister = make_unique<CGBBackgroundPaletteIndexRegister>(&videoController);
    auto cgbBackgroundPaletteIndexRegisterPointer = cgbBackgroundPaletteIndexRegister.get();
    auto cgbBackgroundPaletteDataRegister = make_unique<CGBBackgroundPaletteDataRegister>(&videoController);
    auto cgbBackgroundPaletteDataRegisterPointer = cgbBackgroundPaletteDataRegister.get();

    cgbBackgroundPaletteIndexRegister->RegisterDataRegister(cgbBackgroundPaletteDataRegister.get());
    cgbBackgroundPaletteDataRegister->RegisterIndexRegister(cgbBackgroundPaletteIndexRegister.get());

    controller.RegisterMemoryMappedRegister(std::move(cgbBackgroundPaletteIndexRegister), gbxcore::constants::CGBBackgroundPaletteIndexRegister, Ownership::System);
    controller.RegisterMemoryMappedRegister(std::move(cgbBackgroundPaletteDataRegister), gbxcore::constants::CGBBackgroundPaletteDataRegister, Ownership::System);

    for (auto i = 0; i < 0x3F; ++i)
    {
        controller.Write(static_cast<uint8_t>(i), gbxcore::constants::CGBBackgroundPaletteIndexRegister);
        EXPECT_EQ(static_cast<uint8_t>(i), cgbBackgroundPaletteDataRegisterPointer->CurrentColorIndex());
        EXPECT_FALSE(cgbBackgroundPaletteIndexRegisterPointer->AutoIncrementEnabled());

        EXPECT_CALL(videoController, RegisterCGBBackgroundPaletteColorByte(static_cast<uint8_t>(i), static_cast<uint8_t>(0xFF))).Times(1);
        controller.Write(static_cast<uint8_t>(0xFF), gbxcore::constants::CGBBackgroundPaletteDataRegister);

        EXPECT_EQ(static_cast<uint8_t>(i), cgbBackgroundPaletteDataRegisterPointer->CurrentColorIndex());
        EXPECT_FALSE(cgbBackgroundPaletteIndexRegisterPointer->AutoIncrementEnabled());
    }
}

TEST(CoreTests_VideoControllerRegisters, WriteToGCBBackgroundPaletteDataRegistersWriteAllPalettes2) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto cgbBackgroundPaletteIndexRegister = make_unique<CGBBackgroundPaletteIndexRegister>(&videoController);
    auto cgbBackgroundPaletteIndexRegisterPointer = cgbBackgroundPaletteIndexRegister.get();
    auto cgbBackgroundPaletteDataRegister = make_unique<CGBBackgroundPaletteDataRegister>(&videoController);
    auto cgbBackgroundPaletteDataRegisterPointer = cgbBackgroundPaletteDataRegister.get();

    cgbBackgroundPaletteIndexRegister->RegisterDataRegister(cgbBackgroundPaletteDataRegister.get());
    cgbBackgroundPaletteDataRegister->RegisterIndexRegister(cgbBackgroundPaletteIndexRegister.get());

    controller.RegisterMemoryMappedRegister(std::move(cgbBackgroundPaletteIndexRegister), gbxcore::constants::CGBBackgroundPaletteIndexRegister, Ownership::System);
    controller.RegisterMemoryMappedRegister(std::move(cgbBackgroundPaletteDataRegister), gbxcore::constants::CGBBackgroundPaletteDataRegister, Ownership::System);

    for (auto i = 0; i < 0x3F; ++i)
    {
        controller.Write(static_cast<uint8_t>(i | 0x80), gbxcore::constants::CGBBackgroundPaletteIndexRegister);
        EXPECT_EQ(static_cast<uint8_t>(i), cgbBackgroundPaletteDataRegisterPointer->CurrentColorIndex());
        EXPECT_TRUE(cgbBackgroundPaletteIndexRegisterPointer->AutoIncrementEnabled());

        EXPECT_CALL(videoController, RegisterCGBBackgroundPaletteColorByte(static_cast<uint8_t>(i), static_cast<uint8_t>(0xFF))).Times(1);
        controller.Write(static_cast<uint8_t>(0xFF), gbxcore::constants::CGBBackgroundPaletteDataRegister);

        if (i == 0x3F)
            EXPECT_EQ(static_cast<uint8_t>(0x00), cgbBackgroundPaletteDataRegisterPointer->CurrentColorIndex());
        else
            EXPECT_EQ(static_cast<uint8_t>(i + 1), cgbBackgroundPaletteDataRegisterPointer->CurrentColorIndex());

        EXPECT_TRUE(cgbBackgroundPaletteIndexRegisterPointer->AutoIncrementEnabled());
    }
}

TEST(CoreTests_VideoControllerRegisters, GCBObjectPaletteIndexRegisterConstruction) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto cgbObjectPaletteIndexRegister = make_unique<CGBObjectPaletteIndexRegister>(&videoController);
}

TEST(CoreTests_VideoControllerRegisters, GCBObjectPaletteDataRegisterConstruction) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto cgbObjectPaletteDataRegister = make_unique<CGBObjectPaletteDataRegister>(&videoController);
}

TEST(CoreTests_VideoControllerRegisters, GCBObjectPaletteRegistersRegistration) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto cgbObjectPaletteIndexRegister = make_unique<CGBObjectPaletteIndexRegister>(&videoController);
    auto cgbObjectPaletteDataRegister = make_unique<CGBObjectPaletteDataRegister>(&videoController);

    cgbObjectPaletteIndexRegister->RegisterDataRegister(cgbObjectPaletteDataRegister.get());
    cgbObjectPaletteDataRegister->RegisterIndexRegister(cgbObjectPaletteIndexRegister.get());
}

TEST(CoreTests_VideoControllerRegisters, WriteToGCBObjectPaletteIndexRegisters) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto cgbObjectPaletteIndexRegister = make_unique<CGBObjectPaletteIndexRegister>(&videoController);
    auto cgbObjectPaletteDataRegister = make_unique<CGBObjectPaletteDataRegister>(&videoController);
    auto cgbObjectPaletteDataRegisterPointer = cgbObjectPaletteDataRegister.get();

    cgbObjectPaletteIndexRegister->RegisterDataRegister(cgbObjectPaletteDataRegister.get());
    cgbObjectPaletteDataRegister->RegisterIndexRegister(cgbObjectPaletteIndexRegister.get());

    controller.RegisterMemoryMappedRegister(std::move(cgbObjectPaletteIndexRegister), gbxcore::constants::CGBObjectPaletteIndexRegister, Ownership::System);
    controller.RegisterMemoryMappedRegister(std::move(cgbObjectPaletteDataRegister), gbxcore::constants::CGBObjectPaletteDataRegister, Ownership::System);

    controller.Write(static_cast<uint8_t>(0x07), gbxcore::constants::CGBObjectPaletteIndexRegister);
    EXPECT_EQ(static_cast<uint8_t>(0x07), cgbObjectPaletteDataRegisterPointer->CurrentColorIndex());
}

TEST(CoreTests_VideoControllerRegisters, WriteToGCBObjectPaletteDataRegisters) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto cgbObjectPaletteIndexRegister = make_unique<CGBObjectPaletteIndexRegister>(&videoController);
    auto cgbObjectPaletteDataRegister = make_unique<CGBObjectPaletteDataRegister>(&videoController);
    auto cgbObjectPaletteDataRegisterPointer = cgbObjectPaletteDataRegister.get();

    cgbObjectPaletteIndexRegister->RegisterDataRegister(cgbObjectPaletteDataRegister.get());
    cgbObjectPaletteDataRegister->RegisterIndexRegister(cgbObjectPaletteIndexRegister.get());

    controller.RegisterMemoryMappedRegister(std::move(cgbObjectPaletteIndexRegister), gbxcore::constants::CGBObjectPaletteIndexRegister, Ownership::System);
    controller.RegisterMemoryMappedRegister(std::move(cgbObjectPaletteDataRegister), gbxcore::constants::CGBObjectPaletteDataRegister, Ownership::System);

    controller.Write(static_cast<uint8_t>(0x3F), gbxcore::constants::CGBObjectPaletteIndexRegister);
    EXPECT_EQ(static_cast<uint8_t>(0x3F), cgbObjectPaletteDataRegisterPointer->CurrentColorIndex());

    EXPECT_CALL(videoController, RegisterCGBObjectPaletteColorByte(static_cast<uint8_t>(0x3F), static_cast<uint8_t>(0x65))).Times(1);
    controller.Write(static_cast<uint8_t>(0x65), gbxcore::constants::CGBObjectPaletteDataRegister);
}

TEST(CoreTests_VideoControllerRegisters, WriteToGCBObjectPaletteDataRegistersWithAutoIncrement) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto cgbObjectPaletteIndexRegister = make_unique<CGBObjectPaletteIndexRegister>(&videoController);
    auto cgbObjectPaletteIndexRegisterPointer = cgbObjectPaletteIndexRegister.get();
    auto cgbObjectPaletteDataRegister = make_unique<CGBObjectPaletteDataRegister>(&videoController);
    auto cgbObjectPaletteDataRegisterPointer = cgbObjectPaletteDataRegister.get();

    cgbObjectPaletteIndexRegister->RegisterDataRegister(cgbObjectPaletteDataRegister.get());
    cgbObjectPaletteDataRegister->RegisterIndexRegister(cgbObjectPaletteIndexRegister.get());

    controller.RegisterMemoryMappedRegister(std::move(cgbObjectPaletteIndexRegister), gbxcore::constants::CGBObjectPaletteIndexRegister, Ownership::System);
    controller.RegisterMemoryMappedRegister(std::move(cgbObjectPaletteDataRegister), gbxcore::constants::CGBObjectPaletteDataRegister, Ownership::System);

    controller.Write(static_cast<uint8_t>(0xBA), gbxcore::constants::CGBObjectPaletteIndexRegister);
    EXPECT_EQ(static_cast<uint8_t>(0x3A), cgbObjectPaletteDataRegisterPointer->CurrentColorIndex());
    EXPECT_TRUE(cgbObjectPaletteIndexRegisterPointer->AutoIncrementEnabled());

    EXPECT_CALL(videoController, RegisterCGBObjectPaletteColorByte(static_cast<uint8_t>(0x3A), static_cast<uint8_t>(0xF0))).Times(1);
    controller.Write(static_cast<uint8_t>(0xF0), gbxcore::constants::CGBObjectPaletteDataRegister);

    EXPECT_EQ(static_cast<uint8_t>(0x3B), cgbObjectPaletteDataRegisterPointer->CurrentColorIndex());
    EXPECT_TRUE(cgbObjectPaletteIndexRegisterPointer->AutoIncrementEnabled());
}

TEST(CoreTests_VideoControllerRegisters, WriteToGCBObjectPaletteDataOutOfRange) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto cgbObjectPaletteIndexRegister = make_unique<CGBObjectPaletteIndexRegister>(&videoController);
    auto cgbObjectPaletteIndexRegisterPointer = cgbObjectPaletteIndexRegister.get();
    auto cgbObjectPaletteDataRegister = make_unique<CGBObjectPaletteDataRegister>(&videoController);
    auto cgbObjectPaletteDataRegisterPointer = cgbObjectPaletteDataRegister.get();

    cgbObjectPaletteIndexRegister->RegisterDataRegister(cgbObjectPaletteDataRegister.get());
    cgbObjectPaletteDataRegister->RegisterIndexRegister(cgbObjectPaletteIndexRegister.get());

    controller.RegisterMemoryMappedRegister(std::move(cgbObjectPaletteIndexRegister), gbxcore::constants::CGBObjectPaletteIndexRegister, Ownership::System);
    controller.RegisterMemoryMappedRegister(std::move(cgbObjectPaletteDataRegister), gbxcore::constants::CGBObjectPaletteDataRegister, Ownership::System);

    controller.Write(static_cast<uint8_t>(0xE2), gbxcore::constants::CGBObjectPaletteIndexRegister);
    EXPECT_EQ(static_cast<uint8_t>(0x00), cgbObjectPaletteDataRegisterPointer->CurrentColorIndex());
    EXPECT_TRUE(cgbObjectPaletteIndexRegisterPointer->AutoIncrementEnabled());

    EXPECT_CALL(videoController, RegisterCGBObjectPaletteColorByte(static_cast<uint8_t>(0x00), static_cast<uint8_t>(0x87))).Times(1);
    controller.Write(static_cast<uint8_t>(0x87), gbxcore::constants::CGBObjectPaletteDataRegister);

    EXPECT_EQ(static_cast<uint8_t>(0x01), cgbObjectPaletteDataRegisterPointer->CurrentColorIndex());
    EXPECT_TRUE(cgbObjectPaletteIndexRegisterPointer->AutoIncrementEnabled());
}

TEST(CoreTests_VideoControllerRegisters, WriteToGCBObjectPaletteDataRegistersWriteAllPalettes) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto cgbObjectPaletteIndexRegister = make_unique<CGBObjectPaletteIndexRegister>(&videoController);
    auto cgbObjectPaletteIndexRegisterPointer = cgbObjectPaletteIndexRegister.get();
    auto cgbObjectPaletteDataRegister = make_unique<CGBObjectPaletteDataRegister>(&videoController);
    auto cgbObjectPaletteDataRegisterPointer = cgbObjectPaletteDataRegister.get();

    cgbObjectPaletteIndexRegister->RegisterDataRegister(cgbObjectPaletteDataRegister.get());
    cgbObjectPaletteDataRegister->RegisterIndexRegister(cgbObjectPaletteIndexRegister.get());

    controller.RegisterMemoryMappedRegister(std::move(cgbObjectPaletteIndexRegister), gbxcore::constants::CGBObjectPaletteIndexRegister, Ownership::System);
    controller.RegisterMemoryMappedRegister(std::move(cgbObjectPaletteDataRegister), gbxcore::constants::CGBObjectPaletteDataRegister, Ownership::System);

    for (auto i = 0; i < 0x3F; ++i)
    {
        controller.Write(static_cast<uint8_t>(i), gbxcore::constants::CGBObjectPaletteIndexRegister);
        EXPECT_EQ(static_cast<uint8_t>(i), cgbObjectPaletteDataRegisterPointer->CurrentColorIndex());
        EXPECT_FALSE(cgbObjectPaletteIndexRegisterPointer->AutoIncrementEnabled());

        EXPECT_CALL(videoController, RegisterCGBObjectPaletteColorByte(static_cast<uint8_t>(i), static_cast<uint8_t>(0xFF))).Times(1);
        controller.Write(static_cast<uint8_t>(0xFF), gbxcore::constants::CGBObjectPaletteDataRegister);

        EXPECT_EQ(static_cast<uint8_t>(i), cgbObjectPaletteDataRegisterPointer->CurrentColorIndex());
        EXPECT_FALSE(cgbObjectPaletteIndexRegisterPointer->AutoIncrementEnabled());
    }
}

TEST(CoreTests_VideoControllerRegisters, WriteToGCBObjectPaletteDataRegistersWriteAllPalettes2) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto cgbObjectPaletteIndexRegister = make_unique<CGBObjectPaletteIndexRegister>(&videoController);
    auto cgbObjectPaletteIndexRegisterPointer = cgbObjectPaletteIndexRegister.get();
    auto cgbObjectPaletteDataRegister = make_unique<CGBObjectPaletteDataRegister>(&videoController);
    auto cgbObjectPaletteDataRegisterPointer = cgbObjectPaletteDataRegister.get();

    cgbObjectPaletteIndexRegister->RegisterDataRegister(cgbObjectPaletteDataRegister.get());
    cgbObjectPaletteDataRegister->RegisterIndexRegister(cgbObjectPaletteIndexRegister.get());

    controller.RegisterMemoryMappedRegister(std::move(cgbObjectPaletteIndexRegister), gbxcore::constants::CGBObjectPaletteIndexRegister, Ownership::System);
    controller.RegisterMemoryMappedRegister(std::move(cgbObjectPaletteDataRegister), gbxcore::constants::CGBObjectPaletteDataRegister, Ownership::System);

    for (auto i = 0; i < 0x3F; ++i)
    {
        controller.Write(static_cast<uint8_t>(i | 0x80), gbxcore::constants::CGBObjectPaletteIndexRegister);
        EXPECT_EQ(static_cast<uint8_t>(i), cgbObjectPaletteDataRegisterPointer->CurrentColorIndex());
        EXPECT_TRUE(cgbObjectPaletteIndexRegisterPointer->AutoIncrementEnabled());

        EXPECT_CALL(videoController, RegisterCGBObjectPaletteColorByte(static_cast<uint8_t>(i), static_cast<uint8_t>(0xFF))).Times(1);
        controller.Write(static_cast<uint8_t>(0xFF), gbxcore::constants::CGBObjectPaletteDataRegister);

        if (i == 0x3F)
            EXPECT_EQ(static_cast<uint8_t>(0x00), cgbObjectPaletteDataRegisterPointer->CurrentColorIndex());
        else
            EXPECT_EQ(static_cast<uint8_t>(i + 1), cgbObjectPaletteDataRegisterPointer->CurrentColorIndex());

        EXPECT_TRUE(cgbObjectPaletteIndexRegisterPointer->AutoIncrementEnabled());
    }
}