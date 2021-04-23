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

TEST(CoreTests_MemoryMappedRegister, RegisterMemoryMappedRegister) 
{
    auto interruptEnableRegister = make_unique<InterruptEnableRegister>();
}

TEST(CoreTests_MemoryMappedRegister, RegisterMemoryMappedRegisterInSystemMode) 
{
    auto interruptEnableRegister = make_unique<InterruptEnableRegister>();
    MemoryController controller;

    controller.RegisterMemoryMappedRegister(std::move(interruptEnableRegister), 0xFFFF, Ownership::System);
    controller.Write(static_cast<uint8_t>(0xAA), 0xFFFF);
    
    EXPECT_EQ(get<uint8_t>(controller.Read(0xFFFF, MemoryAccessType::Byte)), static_cast<uint8_t>(0xAA));
}

TEST(CoreTests_MemoryMappedRegister, RegisterMemoryMappedRegisterInUserMode) 
{
    auto interruptEnableRegister = make_unique<InterruptEnableRegister>();
    MemoryController controller;

    controller.RegisterMemoryMappedRegister(std::move(interruptEnableRegister), 0x0190, Ownership::User);
    controller.SetMode(Mode::User);
    controller.Write(static_cast<uint8_t>(0xCC), 0x0190);
    
    EXPECT_EQ(get<uint8_t>(controller.Read(0x0190, MemoryAccessType::Byte)), static_cast<uint8_t>(0xCC));
}

TEST(CoreTests_MemoryMappedRegister, RegisterMemoryMappedRegisterInDualOwner) 
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

TEST(CoreTests_MemoryMappedRegister, UnregisterMemoryMappeddRegisterInSystemMode) 
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

TEST(CoreTests_MemoryMappedRegister, UnregisterMemoryMappeddRegisterInUserMode) 
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

TEST(CoreTests_MemoryMappedRegister, UnregisterMemoryMappeddledRegisterOfBothOwnership) 
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
