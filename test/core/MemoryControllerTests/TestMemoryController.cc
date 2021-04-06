#include <gtest/gtest.h>

#include "CoreTestMocksAndWrappers.h"
#include "TestUtils.h"

#include <iostream>

#include "AddressRange.h"
#include "MemoryController.h"
#include "MemoryControllerInterface.h"
#include "RAM.h"
#include "ROM.h"
#include "SystemMode.h"

using namespace std;
using namespace gbxcore;
using namespace gbxcore::memory;
using namespace gbxcore::interfaces;

TEST(CoreTests_MemoryController, ResourceRegistration) 
{
    MemoryController memController;
    auto ram = make_unique<RAM>(0x100);
    memController.RegisterMemoryResource
    (
        std::move(ram),
        AddressRange(0x0100, 0x0200, RangeType::BeginInclusive),
        Ownership::System
    );

    memController.Write(static_cast<uint8_t>(0xFF), 0x0120);
    auto value = memController.Read(0x0120, MemoryAccessType::Byte);

    EXPECT_EQ(0xFF, get<uint8_t>(value));
}

TEST(CoreTests_MemoryController, ResourceRegistrationAddressDoesNoMatchSize) 
{
    auto test1Succeeded = false;
    auto test2Succeeded = false;

    MemoryController memController;
    auto rom = make_unique<RAM>(0x100);

    try
    {
        memController.RegisterMemoryResource
        (
            std::move(rom),
            AddressRange(0x0100, 0x0300, RangeType::AllInclusive),
            Ownership::System
        );
    }
    catch (const MemoryControllerException& e)
    {
        test1Succeeded = true;
    }

    rom = make_unique<RAM>(0x100);
    try
    {
        memController.RegisterMemoryResource
        (
            std::move(rom),
            AddressRange(0x0100, 0x0110, RangeType::AllInclusive),
            Ownership::System
        );
    }
    catch (const MemoryControllerException& e)
    {
        test2Succeeded = true;
    }

    EXPECT_TRUE(test1Succeeded);
    EXPECT_TRUE(test2Succeeded);
}

TEST(CoreTests_MemoryController, ResourceRegistrationWithOverlap) 
{
    auto overlapDetected = false;
    MemoryController memController;
    auto rom = make_unique<ROM>(0x100);
    auto romOverlapping = make_unique<ROM>(0x1000);

    try
    {
        memController.RegisterMemoryResource
        (
            std::move(rom),
            AddressRange(0x0100, 0x0200, RangeType::AllInclusive),
            Ownership::System
        );

        memController.RegisterMemoryResource
        (
            std::move(romOverlapping),
            AddressRange(0x0110, 0x1110, RangeType::AllInclusive),
            Ownership::System
        );
    }
    catch (const MemoryControllerException& e)
    {
        overlapDetected = true;
    }

    EXPECT_TRUE(overlapDetected);
}

TEST(CoreTests_MemoryController, TwoResourcesOperation) 
{
    MemoryController memController;
    auto smallRAM = make_unique<RAM>(0x100);
    auto largeRAM = make_unique<RAM>(0x200);
    auto smallRAMPointer = smallRAM.get();
    auto largeRAMPointer = largeRAM.get();

    memController.RegisterMemoryResource
    (
        std::move(smallRAM),
        AddressRange(0x0000, 0x0100, RangeType::BeginInclusive),
        Ownership::System
    );

    memController.RegisterMemoryResource
    (
        std::move(largeRAM),
        AddressRange(0x0100, 0x300, RangeType::BeginInclusive),
        Ownership::System
    );

    memController.Write(static_cast<uint16_t>(0xEF87), 0x006A);
    auto value = memController.Read(0x006A, MemoryAccessType::Word);
    EXPECT_EQ(0xEF87, get<uint16_t>(value));
    EXPECT_EQ(0xEF87, get<uint16_t>(smallRAMPointer->Read(0x006A, MemoryAccessType::Word)));

    memController.Write(static_cast<uint16_t>(0x56D2), 0x0211);
    value = memController.Read(0x0211, MemoryAccessType::Word);
    EXPECT_EQ(0x56D2, get<uint16_t>(value));
    EXPECT_EQ(0x56D2, get<uint16_t>(largeRAMPointer->Read(0x0111, MemoryAccessType::Word)));
}

TEST(CoreTests_MemoryController, NonConsecultiveResources) 
{
    MemoryController memController;
    auto smallRAM = make_unique<RAM>(0x100);
    auto largeRAM = make_unique<RAM>(0x200);
    auto smallRAMPointer = smallRAM.get();
    auto largeRAMPointer = largeRAM.get();

    memController.RegisterMemoryResource
    (
        std::move(smallRAM),
        AddressRange(0x0200, 0x0300, RangeType::BeginInclusive),
        Ownership::System
    );
    memController.RegisterMemoryResource
    (
        std::move(largeRAM),
        AddressRange(0xF100, 0xF300, RangeType::BeginInclusive),
        Ownership::System
    );

    memController.Write(static_cast<uint16_t>(0xFFAA), 0x0254);
    auto value = memController.Read(0x0254, MemoryAccessType::Word);
    EXPECT_EQ(0xFFAA, get<uint16_t>(value));
    EXPECT_EQ(0xFFAA, get<uint16_t>(smallRAMPointer->Read(0x0054, MemoryAccessType::Word)));

    memController.Write(static_cast<uint16_t>(0xCCAA), 0xF200);
    value = memController.Read(0xF200, MemoryAccessType::Word);
    EXPECT_EQ(0xCCAA, get<uint16_t>(value));
    EXPECT_EQ(0xCCAA, get<uint16_t>(largeRAMPointer->Read(0x0100, MemoryAccessType::Word)));
}

TEST(CoreTests_MemoryController, AccessEmptyAddressRange) 
{
    auto test1Passed = false;
    auto test2Passed = false;
    MemoryController memController;
    auto rom1 = make_unique<ROM>(0x100);
    auto rom2 = make_unique<ROM>(0x100);

    memController.RegisterMemoryResource
    (
        std::move(rom1),
        AddressRange(0x0200, 0x0300, RangeType::BeginInclusive),
        Ownership::System
    );
    memController.RegisterMemoryResource
    (
        std::move(rom2),
        AddressRange(0x0400, 0x0500, RangeType::BeginInclusive),
        Ownership::System
    );

    try
    {
        memController.Write(static_cast<uint16_t>(0xFFFF), 0x0320);
    }
    catch(const MemoryControllerException& e)
    {
        test1Passed = true;
    }
    
    try
    {
        [[maybe_unused]] auto value = memController.Read(0x03FF, MemoryAccessType::Word);
    }
    catch(const MemoryControllerException& e)
    {
        test2Passed = true;
    }

    EXPECT_TRUE(test1Passed);
    EXPECT_TRUE(test2Passed);
}

TEST(CoreTests_MemoryController, PerformOperationsInTheRangeBorders) 
{
    auto test1Passed = false;
    auto test2Passed = false;
    MemoryController memController;
    auto rom = make_unique<ROM>(0x100);

    memController.RegisterMemoryResource
    (
        std::move(rom),
        AddressRange(0x0200, 0x0300, RangeType::BeginInclusive),
        Ownership::System
    );

    try
    {
        memController.Write(static_cast<uint16_t>(0xFFFF), 0x0300);
    }
    catch(const MemoryControllerException& e)
    {
        test1Passed = true;
    }
    
    try
    {
        [[maybe_unused]] auto nextValue = memController.Read(0x01FF, MemoryAccessType::Word);
    }
    catch(const MemoryControllerException& e)
    {
        test2Passed = true;
    }

    EXPECT_TRUE(test1Passed);
    EXPECT_TRUE(test2Passed);
}

TEST(CoreTests_MemoryController, UnregisterResource) 
{
    MemoryController memController;
    auto rom = make_unique<ROM>(0x100);
    
    auto id = memController.RegisterMemoryResource
    (
        std::move(rom),
        AddressRange(0x0100, 0x0200, RangeType::BeginInclusive),
        Ownership::System
    );

    memController.UnregisterMemoryResource(id, Ownership::System);
}


TEST(CoreTests_MemoryController, UnregisterAFewResource) 
{
    auto finalTestPassed = false;
    MemoryController memController;
    auto rom = make_unique<ROM>(0x100);
    auto ram = make_unique<RAM>(0x100);
    auto ram1 = make_unique<RAM>(0x100);
    auto rom1 = make_unique<ROM>(0x100);
    auto rom2 = make_unique<ROM>(0x100);
    
    auto ram1Pointer = ram1.get();

    memController.RegisterMemoryResource
    (
        std::move(rom),
        AddressRange(0x0000, 0x0100, RangeType::BeginInclusive),
        Ownership::System
    );
    memController.RegisterMemoryResource
    (
        std::move(rom1),
        AddressRange(0x0100, 0x0200, RangeType::BeginInclusive),
        Ownership::System
    );
    memController.RegisterMemoryResource
    (
        std::move(ram),
        AddressRange(0x0200, 0x0300, RangeType::BeginInclusive),
        Ownership::System
    );
    auto ram1ID = memController.RegisterMemoryResource
    (
        std::move(ram1),
        AddressRange(0x0300, 0x0400, RangeType::BeginInclusive),
        Ownership::System
    );
    memController.RegisterMemoryResource
    (
        std::move(rom2),
        AddressRange(0x0400, 0x0500, RangeType::BeginInclusive),
        Ownership::System
    );


    memController.Write(static_cast<uint8_t>(0xFF), 0x0301);
    auto value = memController.Read(0x0301, MemoryAccessType::Byte);
    EXPECT_EQ(0xFF, get<uint8_t>(value));

    memController.UnregisterMemoryResource(ram1ID, Ownership::System);

    try
    {
        value = memController.Read(0x0301, MemoryAccessType::Byte);
    }
    catch(const MemoryControllerException& e)
    {
        finalTestPassed = true;;
    }
    EXPECT_TRUE(finalTestPassed);

    value = ram1Pointer->Read(0x0001, MemoryAccessType::Byte);
    EXPECT_EQ(0xFF, get<uint8_t>(value));   
}

TEST(CoreTests_MemoryController, ReuseUnregisteredRange) 
{
    MemoryController memController;
    auto ram = make_unique<RAM>(0x100);
    auto ram2 = make_unique<RAM>(0x100);
    
    auto ram2Pointer = ram2.get();
    
    auto ram1ID = memController.RegisterMemoryResource
    (
        std::move(ram),
        AddressRange(0x0100, 0x0200, RangeType::BeginInclusive),
        Ownership::System
    );

    memController.Write(static_cast<uint8_t>(0xFF), 0x0101);
    auto value = memController.Read(0x0101, MemoryAccessType::Byte);
    EXPECT_EQ(0xFF, get<uint8_t>(value));

    memController.UnregisterMemoryResource(ram1ID, Ownership::System);

    ram2Pointer->Write(static_cast<uint8_t>(0xDD), 0x0001);

    memController.RegisterMemoryResource
    (
        std::move(ram2),
        AddressRange(0x0100, 0x0200, RangeType::BeginInclusive),
        Ownership::System
    );

    value = memController.Read(0x0101, MemoryAccessType::Byte);
    EXPECT_EQ(0xDD, get<uint8_t>(value));
}

TEST(CoreTests_MemoryController, LoadMemoryResource) 
{
    MemoryController memController;
    auto rom = make_unique<ROM>(0x010);

    memController.RegisterMemoryResource
    (
        std::move(rom),
        AddressRange(0x0100, 0x0110, RangeType::BeginInclusive),
        Ownership::System
    );

    array<uint8_t, 0x10> romContent = {0xAA, 0xAA, 0xAA, 0xAA, 
                                       0xAA, 0xAA, 0xAA, 0xAA, 
                                       0xAA, 0xAA, 0xAA, 0xAA, 
                                       0xAA, 0xAA, 0xAA, 0xAA};

    memController.Load(make_unique<uint8_t*>(romContent.data()), romContent.size(), 0x0100, nullopt);
    
    auto address = 0x0100;
    for (auto element : romContent)
    {
        auto readValue = memController.Read(address++, MemoryAccessType::Byte);
        EXPECT_EQ(element, get<uint8_t>(readValue));
    }
}

TEST(CoreTests_MemoryController, LoadMemoryResourceAtWrongLocation) 
{
    auto testPassed = false;
    MemoryController memController;
    auto rom = make_unique<ROM>(0x010);
    
    memController.RegisterMemoryResource
    (
        std::move(rom),
        AddressRange(0x0100, 0x0110, RangeType::BeginInclusive),
        Ownership::System
    );

    array<uint8_t, 0x10> romContent = {0xAA, 0xAA, 0xAA, 0xAA, 
                                       0xAA, 0xAA, 0xAA, 0xAA, 
                                       0xAA, 0xAA, 0xAA, 0xAA, 
                                       0xAA, 0xAA, 0xAA, 0xAA};
    try
    {
       memController.Load(make_unique<uint8_t*>(romContent.data()), romContent.size(), 0x0000, nullopt);
    }
    catch(const MemoryControllerException& e)
    {
        testPassed = true;
    }

    EXPECT_TRUE(testPassed);
}


TEST(CoreTests_MemoryController, LoadMemoryResourceWithOffset) 
{
    MemoryController memController;
    auto rom = make_unique<ROM>(0x010);
    auto romPtr = rom.get();
    
    memController.RegisterMemoryResource
    (
        std::move(rom),
        AddressRange(0x0100, 0x0110, RangeType::BeginInclusive),
        Ownership::System
    );

    array<uint8_t, 0x10> romContent = {0xAA, 0xAA, 0xAA, 0xAA, 
                                       0xAA, 0xAA, 0xAA, 0xAA, 
                                       0xAA, 0xAA, 0xAA, 0xAA, 
                                       0xAA, 0xAA, 0xAA, 0xAA};

    array<uint8_t, 0x08> offsetContent = {0xBB, 0xBB, 0xBB, 0xBB, 
                                          0xBB, 0xBB, 0xBB, 0xBB};

    memController.Load(make_unique<uint8_t*>(romContent.data()), romContent.size(), 0x0100, nullopt);
    memController.Load(make_unique<uint8_t*>(offsetContent.data()), offsetContent.size(), 0x0100, make_optional<size_t>(0x08));
    
    for (auto i = static_cast<size_t>(0x0100); i < romPtr->Size(); i++)
    {
        auto readValue = memController.Read(i, MemoryAccessType::Byte);

        if (i < 0x08)
            EXPECT_EQ(romContent[i], get<uint8_t>(readValue));
        else
            EXPECT_EQ(offsetContent[i - 0x08], get<uint8_t>(readValue));
    }
}

TEST(CoreTests_MemoryController, WriteToRadOnlyRange) 
{
    MemoryController memController;
    auto rom = make_unique<ROM>(0x100);
    
    memController.RegisterMemoryResource
    (
        std::move(rom),
        AddressRange(0x0100, 0x0200, RangeType::BeginInclusive),
        Ownership::System
    );

    ASSERT_EXCEPTION( { memController.Write(static_cast<uint8_t>(0xFF), 0x0120); }, 
                      MemoryAccessException, 
                      "Attempted to write to a read-only resource");
}

TEST(CoreTests_MemoryController, ChangeMemoryMapMode) 
{
    array<uint8_t, 0x10> systemContent = {0xAA, 0xAA, 0xAA, 0xAA, 
                                          0xAA, 0xAA, 0xAA, 0xAA, 
                                          0xAA, 0xAA, 0xAA, 0xAA, 
                                          0xAA, 0xAA, 0xAA, 0xAA};
    
    array<uint8_t, 0x10> userContent = {  0xBB, 0xBB, 0xBB, 0xBB, 
                                          0xBB, 0xBB, 0xBB, 0xBB, 
                                          0xBB, 0xBB, 0xBB, 0xBB, 
                                          0xBB, 0xBB, 0xBB, 0xBB};

    MemoryController memController;
    auto systemROM = make_unique<ROM>(0x100);
    auto userROM = make_unique<ROM>(0x100);
    
    systemROM->Load(make_shared<uint8_t*>(systemContent.data()), 0x10, std::nullopt);
    userROM->Load(make_shared<uint8_t*>(userContent.data()), 0x10, std::nullopt);

    memController.RegisterMemoryResource
    (
        std::move(systemROM),
        AddressRange(0x0000, 0x0100, RangeType::BeginInclusive),
        Ownership::System
    );
    
    memController.RegisterMemoryResource
    (
        std::move(userROM),
        AddressRange(0x0000, 0x0100, RangeType::BeginInclusive),
        Ownership::User
    );

    memController.SetMode(Mode::System);
    EXPECT_EQ(Mode::System, memController.Mode());

    for (auto i = 0llu; i < 0x10llu; ++i)
    {
        auto systemValue = memController.Read(static_cast<uint16_t>(i), MemoryAccessType::Byte);
        EXPECT_EQ(*(systemContent.begin() + i), get<uint8_t>(systemValue));
    }
    
    memController.SetMode(Mode::User);
    EXPECT_EQ(Mode::User, memController.Mode());
    
    for (auto i = 0llu; i < 0x10llu; ++i)
    {
        auto userValue = memController.Read(static_cast<uint16_t>(i), MemoryAccessType::Byte);
        EXPECT_EQ(*(userContent.begin() + i), get<uint8_t>(userValue));
    }
}