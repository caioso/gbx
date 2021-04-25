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
#include "LCDVideoController.h"
#include "MemoryController.h"
#include "MemoryMappedRegister.h"
#include "RAM.h"
#include "VideoOutputInterface.h"

using namespace std; 
using namespace gbxcore;
using namespace gbxcore::interfaces;
using namespace gbxcore::memory;
using namespace gbxcore::memory::registers;
using namespace gbxcore::video;

TEST(CoreTests_LCDVideoController, LCDVideoControllerConstruction) 
{
    VideoOutputMock outputMock;
    LCDVideoController videoController(&outputMock);
}

TEST(CoreTests_LCDVideoController, LCDVideoControllerInitialStatus) 
{
    VideoOutputMock outputMock;
    LCDVideoController videoController(&outputMock);

    EXPECT_FALSE(videoController.IsVideoEnabled());
    EXPECT_FALSE(videoController.IsWindowVisible());
    EXPECT_FALSE(videoController.IsBackgroundVisible());
    EXPECT_FALSE(videoController.AreSpritesVisible());
}

TEST(CoreTests_LCDVideoController, LCDVideoControllerInitializationWithVideoOutput) 
{
    VideoOutputMock outputMock;
    LCDVideoController videoController(&outputMock);
}

TEST(CoreTests_LCDVideoController, LCDVideoControllerEnableVideo) 
{
    VideoOutputMock outputMock;
    LCDVideoController videoController(&outputMock);

    EXPECT_CALL(outputMock, SetVideoEnable(true)).Times(1);
    videoController.EnableVideo();
}

TEST(CoreTests_LCDVideoController, LCDVideoControllerDisableVideo) 
{
    VideoOutputMock outputMock;
    LCDVideoController videoController(&outputMock);

    EXPECT_CALL(outputMock, SetVideoEnable(false)).Times(1);
    videoController.DisableVideo();
}

TEST(CoreTests_LCDVideoController, LCDVideoControllerEnableWindow) 
{
    VideoOutputMock outputMock;
    LCDVideoController videoController(&outputMock);

    EXPECT_CALL(outputMock, SetWindowEnable(true)).Times(1);
    videoController.ShowWindow();
}

TEST(CoreTests_LCDVideoController, LCDVideoControllerDisableWindow) 
{
    VideoOutputMock outputMock;
    LCDVideoController videoController(&outputMock);

    EXPECT_CALL(outputMock, SetWindowEnable(false)).Times(1);
    videoController.HideWindow();
}

TEST(CoreTests_LCDVideoController, LCDVideoControllerEnableBackground) 
{
    VideoOutputMock outputMock;
    LCDVideoController videoController(&outputMock);

    EXPECT_CALL(outputMock, SetBackgroundEnable(true)).Times(1);
    videoController.ShowBackground();
}

TEST(CoreTests_LCDVideoController, LCDVideoControllerDisableBackground) 
{
    VideoOutputMock outputMock;
    LCDVideoController videoController(&outputMock);

    EXPECT_CALL(outputMock, SetBackgroundEnable(false)).Times(1);
    videoController.HideBackground();
}

TEST(CoreTests_LCDVideoController, LCDVideoControllerEnableSprites) 
{
    VideoOutputMock outputMock;
    LCDVideoController videoController(&outputMock);

    EXPECT_CALL(outputMock, SetSpriteEnable(true)).Times(1);
    videoController.ShowSprites();
}

TEST(CoreTests_LCDVideoController, LCDVideoControllerDisableSprites) 
{
    VideoOutputMock outputMock;
    LCDVideoController videoController(&outputMock);

    EXPECT_CALL(outputMock, SetSpriteEnable(false)).Times(1);
    videoController.HideSprites();
}

TEST(CoreTests_LCDVideoController, LCDVideoControllerSetWindowTileMapBase) 
{
    VideoOutputMock outputMock;
    LCDVideoController videoController(&outputMock);

    EXPECT_CALL(outputMock, SetWindowTileMapBaseAddress(0x9C00llu)).Times(1);
    videoController.SelectWindowTileMap(0x9C00llu);
    EXPECT_CALL(outputMock, SetWindowTileMapBaseAddress(0x9800llu)).Times(1);
    videoController.SelectWindowTileMap(0x9800llu);
}

TEST(CoreTests_LCDVideoController, LCDVideoControllerSetBackgroundTileMapBase) 
{
    VideoOutputMock outputMock;
    LCDVideoController videoController(&outputMock);

    EXPECT_CALL(outputMock, SetBackgroundTileMapBaseAddress(0x9C00llu)).Times(1);
    videoController.SelectBackgroundTileMap(0x9C00llu);
    EXPECT_CALL(outputMock, SetBackgroundTileMapBaseAddress(0x9800llu)).Times(1);
    videoController.SelectBackgroundTileMap(0x9800llu);
}

TEST(CoreTests_LCDVideoController, LCDVideoControllerSetSpriteMode) 
{
    VideoOutputMock outputMock;
    LCDVideoController videoController(&outputMock);

    EXPECT_CALL(outputMock, SetSpriteMode(gbxcore::interfaces::SpriteMode::Sprite8x8)).Times(1);
    videoController.SetSpriteMode(0);
    EXPECT_CALL(outputMock, SetSpriteMode(gbxcore::interfaces::SpriteMode::Sprite8x16)).Times(1);
    videoController.SetSpriteMode(1);
}    

TEST(CoreTests_LCDVideoController, LCDVideoControllerScrollBackground) 
{
    VideoOutputMock outputMock;
    LCDVideoController videoController(&outputMock);

    EXPECT_CALL(outputMock, SetBackgroundScrolXY(0x01llu, 0x00llu)).Times(1);
    videoController.ScrollBackgroundX(0x01llu);
    EXPECT_CALL(outputMock, SetBackgroundScrolXY(0x01llu, 0x04llu)).Times(1);
    videoController.ScrollBackgroundY(0x04llu);
}    

TEST(CoreTests_LCDVideoController, LCDVideoControllerScrollWindow) 
{
    VideoOutputMock outputMock;
    LCDVideoController videoController(&outputMock);

    EXPECT_CALL(outputMock, SetWindowScrolXY(0xAAllu, 0x00llu)).Times(1);
    videoController.ScrollWindowX(0xAAllu);
    EXPECT_CALL(outputMock, SetWindowScrolXY(0xAAllu, 0xBBllu)).Times(1);
    videoController.ScrollWindowY(0xBBllu);
}    
