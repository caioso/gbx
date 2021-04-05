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
    shared_ptr<MemoryInterface> ram(new RAM(0x100));
    memController.RegisterMemoryResource
    (
        ram,
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
    shared_ptr<MemoryInterface> rom(new ROM(0x100));

    try
    {
        memController.RegisterMemoryResource
        (
            rom,
            AddressRange(0x0100, 0x0300, RangeType::AllInclusive),
            Ownership::System
        );
    }
    catch (const MemoryControllerException& e)
    {
        test1Succeeded = true;
    }

    try
    {
        memController.RegisterMemoryResource
        (
            rom,
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
    shared_ptr<MemoryInterface> rom(new ROM(0x100));
    shared_ptr<MemoryInterface> romOverlapping(new ROM(0x1000));

    try
    {
        memController.RegisterMemoryResource
        (
            rom,
            AddressRange(0x0100, 0x0200, RangeType::AllInclusive),
            Ownership::System
        );

        memController.RegisterMemoryResource
        (
            romOverlapping,
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
    shared_ptr<MemoryInterface> smallRAM(new RAM(0x100));
    shared_ptr<MemoryInterface> largeRAM(new RAM(0x200));

    memController.RegisterMemoryResource
    (
        smallRAM,
        AddressRange(0x0000, 0x0100, RangeType::BeginInclusive),
        Ownership::System
    );
    memController.RegisterMemoryResource
    (
        largeRAM,
        AddressRange(0x0100, 0x300, RangeType::BeginInclusive),
        Ownership::System
    );

    memController.Write(static_cast<uint16_t>(0xEF87), 0x006A);
    auto value = memController.Read(0x006A, MemoryAccessType::Word);
    EXPECT_EQ(0xEF87, get<uint16_t>(value));
    EXPECT_EQ(0xEF87, get<uint16_t>(smallRAM.get()->Read(0x006A, MemoryAccessType::Word)));

    memController.Write(static_cast<uint16_t>(0x56D2), 0x0211);
    value = memController.Read(0x0211, MemoryAccessType::Word);
    EXPECT_EQ(0x56D2, get<uint16_t>(value));
    EXPECT_EQ(0x56D2, get<uint16_t>(largeRAM.get()->Read(0x0111, MemoryAccessType::Word)));
}

TEST(CoreTests_MemoryController, NonConsecultiveResources) 
{
    MemoryController memController;
    shared_ptr<MemoryInterface> smallRAM(new RAM(0x100));
    shared_ptr<MemoryInterface> largeRAM(new RAM(0x200));

    memController.RegisterMemoryResource
    (
        smallRAM,
        AddressRange(0x0200, 0x0300, RangeType::BeginInclusive),
        Ownership::System
    );
    memController.RegisterMemoryResource
    (
        largeRAM,
        AddressRange(0xF100, 0xF300, RangeType::BeginInclusive),
        Ownership::System
    );

    memController.Write(static_cast<uint16_t>(0xFFAA), 0x0254);
    auto value = memController.Read(0x0254, MemoryAccessType::Word);
    EXPECT_EQ(0xFFAA, get<uint16_t>(value));
    EXPECT_EQ(0xFFAA, get<uint16_t>(smallRAM.get()->Read(0x0054, MemoryAccessType::Word)));

    memController.Write(static_cast<uint16_t>(0xCCAA), 0xF200);
    value = memController.Read(0xF200, MemoryAccessType::Word);
    EXPECT_EQ(0xCCAA, get<uint16_t>(value));
    EXPECT_EQ(0xCCAA, get<uint16_t>(largeRAM.get()->Read(0x0100, MemoryAccessType::Word)));
}

TEST(CoreTests_MemoryController, AccessEmptyAddressRange) 
{
    auto test1Passed = false;
    auto test2Passed = false;
    MemoryController memController;
    shared_ptr<MemoryInterface> rom1(new ROM(0x100));
    shared_ptr<MemoryInterface> rom2(new ROM(0x100));

    memController.RegisterMemoryResource
    (
        rom1,
        AddressRange(0x0200, 0x0300, RangeType::BeginInclusive),
        Ownership::System
    );
    memController.RegisterMemoryResource
    (
        rom2,
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
    shared_ptr<MemoryInterface> rom(new ROM(0x100));

    memController.RegisterMemoryResource
    (
        rom,
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
    shared_ptr<MemoryInterface> rom(new ROM(0x100));
    memController.RegisterMemoryResource
    (
        rom,
        AddressRange(0x0100, 0x0200, RangeType::BeginInclusive),
        Ownership::System
    );

    memController.UnregisterMemoryResource(rom, Ownership::System);
}


TEST(CoreTests_MemoryController, UnregisterAFewResource) 
{
    auto finalTestPassed = false;
    MemoryController memController;
    shared_ptr<MemoryInterface> rom(new ROM(0x100));
    shared_ptr<MemoryInterface> ram(new RAM(0x100));
    shared_ptr<MemoryInterface> ram1(new RAM(0x100));
    shared_ptr<MemoryInterface> rom1(new ROM(0x100));
    shared_ptr<MemoryInterface> rom2(new ROM(0x100));

    memController.RegisterMemoryResource
    (
        rom,
        AddressRange(0x0000, 0x0100, RangeType::BeginInclusive),
        Ownership::System
    );
    memController.RegisterMemoryResource
    (
        rom1,
        AddressRange(0x0100, 0x0200, RangeType::BeginInclusive),
        Ownership::System
    );
    memController.RegisterMemoryResource
    (
        ram,
        AddressRange(0x0200, 0x0300, RangeType::BeginInclusive),
        Ownership::System
    );
    memController.RegisterMemoryResource
    (
        ram1,
        AddressRange(0x0300, 0x0400, RangeType::BeginInclusive),
        Ownership::System
    );
    memController.RegisterMemoryResource
    (
        rom2,
        AddressRange(0x0400, 0x0500, RangeType::BeginInclusive),
        Ownership::System
    );


    memController.Write(static_cast<uint8_t>(0xFF), 0x0301);
    auto value = memController.Read(0x0301, MemoryAccessType::Byte);
    EXPECT_EQ(0xFF, get<uint8_t>(value));

    memController.UnregisterMemoryResource(ram1, Ownership::System);

    try
    {
        value = memController.Read(0x0301, MemoryAccessType::Byte);
    }
    catch(const MemoryControllerException& e)
    {
        finalTestPassed = true;;
    }
    EXPECT_TRUE(finalTestPassed);

    value = ram1.get()->Read(0x0001, MemoryAccessType::Byte);
    EXPECT_EQ(0xFF, get<uint8_t>(value));   
}

TEST(CoreTests_MemoryController, ReuseUnregisteredRange) 
{
    MemoryController memController;
    shared_ptr<MemoryInterface> ram(new RAM(0x100));
    shared_ptr<MemoryInterface> ram2(new RAM(0x100));
    memController.RegisterMemoryResource
    (
        ram,
        AddressRange(0x0100, 0x0200, RangeType::BeginInclusive),
        Ownership::System
    );

    memController.Write(static_cast<uint8_t>(0xFF), 0x0101);
    auto value = memController.Read(0x0101, MemoryAccessType::Byte);
    EXPECT_EQ(0xFF, get<uint8_t>(value));

    memController.UnregisterMemoryResource(ram, Ownership::System);

    ram2.get()->Write(static_cast<uint8_t>(0xDD), 0x0001);

    memController.RegisterMemoryResource
    (
        ram2,
        AddressRange(0x0100, 0x0200, RangeType::BeginInclusive),
        Ownership::System
    );

    value = memController.Read(0x0101, MemoryAccessType::Byte);
    EXPECT_EQ(0xDD, get<uint8_t>(value));
}

TEST(CoreTests_MemoryController, LoadMemoryResource) 
{
    MemoryController memController;
    shared_ptr<MemoryInterface> rom(new ROM(0x010));
    memController.RegisterMemoryResource
    (
        rom,
        AddressRange(0x0100, 0x0110, RangeType::BeginInclusive),
        Ownership::System
    );

    array<uint8_t, 0x10> romContent = {0xAA, 0xAA, 0xAA, 0xAA, 
                                       0xAA, 0xAA, 0xAA, 0xAA, 
                                       0xAA, 0xAA, 0xAA, 0xAA, 
                                       0xAA, 0xAA, 0xAA, 0xAA};

    memController.Load(make_shared<uint8_t*>(romContent.data()), romContent.size(), 0x0100, nullopt);
    
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
    shared_ptr<MemoryInterface> rom(new ROM(0x010));
    memController.RegisterMemoryResource
    (
        rom,
        AddressRange(0x0100, 0x0110, RangeType::BeginInclusive),
        Ownership::System
    );

    array<uint8_t, 0x10> romContent = {0xAA, 0xAA, 0xAA, 0xAA, 
                                       0xAA, 0xAA, 0xAA, 0xAA, 
                                       0xAA, 0xAA, 0xAA, 0xAA, 
                                       0xAA, 0xAA, 0xAA, 0xAA};
    try
    {
       memController.Load(make_shared<uint8_t*>(romContent.data()), romContent.size(), 0x0000, nullopt);
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
    shared_ptr<MemoryInterface> rom(new ROM(0x010));
    memController.RegisterMemoryResource
    (
        rom,
        AddressRange(0x0100, 0x0110, RangeType::BeginInclusive),
        Ownership::System
    );

    array<uint8_t, 0x10> romContent = {0xAA, 0xAA, 0xAA, 0xAA, 
                                       0xAA, 0xAA, 0xAA, 0xAA, 
                                       0xAA, 0xAA, 0xAA, 0xAA, 
                                       0xAA, 0xAA, 0xAA, 0xAA};

    array<uint8_t, 0x08> offsetContent = {0xBB, 0xBB, 0xBB, 0xBB, 
                                          0xBB, 0xBB, 0xBB, 0xBB};

    memController.Load(make_shared<uint8_t*>(romContent.data()), romContent.size(), 0x0100, nullopt);
    memController.Load(make_shared<uint8_t*>(offsetContent.data()), offsetContent.size(), 0x0100, make_optional<size_t>(0x08));
    
    for (auto i = static_cast<size_t>(0x0100); i < rom->Size(); i++)
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
    shared_ptr<MemoryInterface> rom(new ROM(0x100));
    memController.RegisterMemoryResource
    (
        rom,
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
    shared_ptr<MemoryInterface> systemROM(new ROM(0x100));
    shared_ptr<MemoryInterface> userROM(new ROM(0x100));
    
    systemROM->Load(make_shared<uint8_t*>(systemContent.data()), 0x10, std::nullopt);
    userROM->Load(make_shared<uint8_t*>(userContent.data()), 0x10, std::nullopt);

    memController.RegisterMemoryResource
    (
        systemROM,
        AddressRange(0x0000, 0x0100, RangeType::BeginInclusive),
        Ownership::System
    );
    
    memController.RegisterMemoryResource
    (
        userROM,
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