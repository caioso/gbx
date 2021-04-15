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

TEST(CoreTests_MemoryMappedRegister, RegisterMemoryMappedRegisterTwiceAtTheSameAddress) 
{
    auto interruptEnableRegister = make_unique<InterruptEnableRegister>();
    MemoryController controller;

    controller.RegisterMemoryMappedRegister(std::move(interruptEnableRegister), 0xFFFF, Ownership::System);
   
    ASSERT_EXCEPTION( { controller.RegisterMemoryMappedRegister(std::move(interruptEnableRegister), 0xFFFF, Ownership::System); }, 
                      MemoryControllerException, 
                      "Register '65535' has already been registered");
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

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDCRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, EnableVideo());
    
    controller.Write(static_cast<uint8_t>(0x80), gbxcore::constants::LCDCRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x80), get<uint8_t>(controller.Read(gbxcore::constants::LCDCRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterDisableLCD) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDCRegisterAddress, Ownership::System);
    
    // By Default, the LCD is off. Switching it off will have no effect
    EXPECT_CALL(videoController, DisableVideo()).Times(0);
    
    controller.Write(static_cast<uint8_t>(0x00), gbxcore::constants::LCDCRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDCRegisterAddress, MemoryAccessType::Byte)));
}

TEST(CoreTests_MemoryMappedRegister, LCDControlRegisterEnableAndDisableLCD) 
{
    VideoControllerMock videoController;
    MemoryController controller;

    auto lcdControlRegister = make_unique<LCDControlRegister>(&videoController);

    controller.RegisterMemoryMappedRegister(std::move(lcdControlRegister), gbxcore::constants::LCDCRegisterAddress, Ownership::System);
    
    EXPECT_CALL(videoController, EnableVideo());
    
    controller.Write(static_cast<uint8_t>(0x80), gbxcore::constants::LCDCRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x80), get<uint8_t>(controller.Read(gbxcore::constants::LCDCRegisterAddress, MemoryAccessType::Byte)));

    EXPECT_CALL(videoController, DisableVideo());
    
    controller.Write(static_cast<uint8_t>(0x00), gbxcore::constants::LCDCRegisterAddress);   
    
    EXPECT_EQ(static_cast<uint8_t>(0x00), get<uint8_t>(controller.Read(gbxcore::constants::LCDCRegisterAddress, MemoryAccessType::Byte)));
}