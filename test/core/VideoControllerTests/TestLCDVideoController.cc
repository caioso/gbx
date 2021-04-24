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
#include "LCDVideoController.h"

using namespace std; 
using namespace gbxcore;
using namespace gbxcore::interfaces;
using namespace gbxcore::memory;
using namespace gbxcore::memory::registers;
using namespace gbxcore::video;

TEST(CoreTests_LCDVideoController, LCDVideoControllerConstruction) 
{
    LCDVideoController videoController;
}

TEST(CoreTests_LCDVideoController, LCDVideoControllerInitialStatus) 
{
    LCDVideoController videoController;

    EXPECT_FALSE(videoController.IsVideoEnabled());
    EXPECT_FALSE(videoController.IsWindowVisible());
    EXPECT_FALSE(videoController.IsBackgroundVisible());
    EXPECT_FALSE(videoController.AreSpritesVisible());
}
