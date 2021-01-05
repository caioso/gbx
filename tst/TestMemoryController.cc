#include <gtest/gtest.h>

#include <iostream>

#include "../src/AddressRange.h"
#include "../src/MemoryController.h"
#include "../src/interfaces/MemoryControllerInterface.h"
#include "../src/ROM.h"

using namespace std;
using namespace gbx;
using namespace gbx::interfaces;

TEST(TestMemoryController, ResourceRegistration) 
{
    MemoryController memController;
    shared_ptr<MemoryInterface> rom(new ROM(0x100));
    memController.RegisterMemoryResource
    (
        rom,
        AddressRange(0x0100, 0x0200, RangeType::BeginInclusive)
    );

    memController.Write(static_cast<uint8_t>(0xFF), 0x0120);
    auto value = memController.Read(0x0120, MemoryAccessType::Byte);

    EXPECT_EQ(0xFF, get<uint8_t>(value));
}

TEST(TestMemoryController, ResourceRegistrationAddressDoesNoMatchSize) 
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
            AddressRange(0x0100, 0x0300, RangeType::AllInclusive)
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
            AddressRange(0x0100, 0x0110, RangeType::AllInclusive)
        );
    }
    catch (const MemoryControllerException& e)
    {
        test2Succeeded = true;
    }

    EXPECT_TRUE(test1Succeeded);
    EXPECT_TRUE(test2Succeeded);
}

TEST(TestMemoryController, ResourceRegistrationWithOverlap) 
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
            AddressRange(0x0100, 0x0200, RangeType::AllInclusive)
        );

        memController.RegisterMemoryResource
        (
            romOverlapping,
            AddressRange(0x0110, 0x1110, RangeType::AllInclusive)
        );
    }
    catch (const MemoryControllerException& e)
    {
        overlapDetected = true;
    }

    EXPECT_TRUE(overlapDetected);
}

TEST(TestMemoryController, TwoResourcesOperation) 
{
    MemoryController memController;
    shared_ptr<MemoryInterface> smallROM(new ROM(0x100));
    shared_ptr<MemoryInterface> largeROM(new ROM(0x200));

    memController.RegisterMemoryResource
    (
        smallROM,
        AddressRange(0x0000, 0x0100, RangeType::BeginInclusive)
    );
    memController.RegisterMemoryResource
    (
        largeROM,
        AddressRange(0x0100, 0x300, RangeType::BeginInclusive)
    );

    memController.Write(static_cast<uint16_t>(0xEF87), 0x006A);
    auto value = memController.Read(0x006A, MemoryAccessType::Word);
    EXPECT_EQ(0xEF87, get<uint16_t>(value));
    EXPECT_EQ(0xEF87, get<uint16_t>(smallROM.get()->Read(0x006A, MemoryAccessType::Word)));

    memController.Write(static_cast<uint16_t>(0x56D2), 0x0211);
    value = memController.Read(0x0211, MemoryAccessType::Word);
    EXPECT_EQ(0x56D2, get<uint16_t>(value));
    EXPECT_EQ(0x56D2, get<uint16_t>(largeROM.get()->Read(0x0111, MemoryAccessType::Word)));
}

TEST(TestMemoryController, NonConsecultiveResources) 
{
    MemoryController memController;
    shared_ptr<MemoryInterface> smallROM(new ROM(0x100));
    shared_ptr<MemoryInterface> largeROM(new ROM(0x200));

    memController.RegisterMemoryResource
    (
        smallROM,
        AddressRange(0x0200, 0x0300, RangeType::BeginInclusive)
    );
    memController.RegisterMemoryResource
    (
        largeROM,
        AddressRange(0xF100, 0xF300, RangeType::BeginInclusive)
    );

    memController.Write(static_cast<uint16_t>(0xFFAA), 0x0254);
    auto value = memController.Read(0x0254, MemoryAccessType::Word);
    EXPECT_EQ(0xFFAA, get<uint16_t>(value));
    EXPECT_EQ(0xFFAA, get<uint16_t>(smallROM.get()->Read(0x0054, MemoryAccessType::Word)));

    memController.Write(static_cast<uint16_t>(0xCCAA), 0xF200);
    value = memController.Read(0xF200, MemoryAccessType::Word);
    EXPECT_EQ(0xCCAA, get<uint16_t>(value));
    EXPECT_EQ(0xCCAA, get<uint16_t>(largeROM.get()->Read(0x0100, MemoryAccessType::Word)));
}

TEST(TestMemoryController, AccessEmptyAddressRange) 
{
    auto test1Passed = false;
    auto test2Passed = false;
    MemoryController memController;
    shared_ptr<MemoryInterface> rom1(new ROM(0x100));
    shared_ptr<MemoryInterface> rom2(new ROM(0x100));

    memController.RegisterMemoryResource
    (
        rom1,
        AddressRange(0x0200, 0x0300, RangeType::BeginInclusive)
    );
    memController.RegisterMemoryResource
    (
        rom2,
        AddressRange(0x0400, 0x0500, RangeType::BeginInclusive)
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
        memController.Read(0x03FF, MemoryAccessType::Word);
    }
    catch(const MemoryControllerException& e)
    {
        test2Passed = true;
    }

    EXPECT_TRUE(test1Passed);
    EXPECT_TRUE(test2Passed);
}

TEST(TestMemoryController, PerformOperationsInTheRangeBorders) 
{
    auto test1Passed = false;
    auto test2Passed = false;
    MemoryController memController;
    shared_ptr<MemoryInterface> rom(new ROM(0x100));

    memController.RegisterMemoryResource
    (
        rom,
        AddressRange(0x0200, 0x0300, RangeType::BeginInclusive)
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
        memController.Read(0x01FF, MemoryAccessType::Word);
    }
    catch(const MemoryControllerException& e)
    {
        test2Passed = true;
    }

    EXPECT_TRUE(test1Passed);
    EXPECT_TRUE(test2Passed);
}

TEST(TestMemoryController, UnregisterResource) 
{
    MemoryController memController;
    shared_ptr<MemoryInterface> rom(new ROM(0x100));
    memController.RegisterMemoryResource
    (
        rom,
        AddressRange(0x0100, 0x0200, RangeType::BeginInclusive)
    );

    memController.UnregisterMemoryResource(rom);
}


TEST(TestMemoryController, UnregisterAFewResource) 
{
    auto finalTestPassed = false;
    MemoryController memController;
    shared_ptr<MemoryInterface> rom(new ROM(0x100));
    shared_ptr<MemoryInterface> rom1(new ROM(0x100));
    shared_ptr<MemoryInterface> rom2(new ROM(0x100));
    shared_ptr<MemoryInterface> rom3(new ROM(0x100));
    shared_ptr<MemoryInterface> rom4(new ROM(0x100));

    memController.RegisterMemoryResource
    (
        rom,
        AddressRange(0x0000, 0x0100, RangeType::BeginInclusive)
    );
    memController.RegisterMemoryResource
    (
        rom1,
        AddressRange(0x0100, 0x0200, RangeType::BeginInclusive)
    );
    memController.RegisterMemoryResource
    (
        rom2,
        AddressRange(0x0200, 0x0300, RangeType::BeginInclusive)
    );
    memController.RegisterMemoryResource
    (
        rom3,
        AddressRange(0x0300, 0x0400, RangeType::BeginInclusive)
    );
    memController.RegisterMemoryResource
    (
        rom4,
        AddressRange(0x0400, 0x0500, RangeType::BeginInclusive)
    );


    memController.Write(static_cast<uint8_t>(0xFF), 0x0301);
    auto value = memController.Read(0x0301, MemoryAccessType::Byte);
    EXPECT_EQ(0xFF, get<uint8_t>(value));

    memController.UnregisterMemoryResource(rom3);

    try
    {
        value = memController.Read(0x0301, MemoryAccessType::Byte);
    }
    catch(const MemoryControllerException& e)
    {
        finalTestPassed = true;;
    }
    EXPECT_TRUE(finalTestPassed);

    value = rom3.get()->Read(0x0001, MemoryAccessType::Byte);
    EXPECT_EQ(0xFF, get<uint8_t>(value));   
}

TEST(TestMemoryController, ReuseUnregisteredRange) 
{
    MemoryController memController;
    shared_ptr<MemoryInterface> rom(new ROM(0x100));
    shared_ptr<MemoryInterface> rom2(new ROM(0x100));
    memController.RegisterMemoryResource
    (
        rom,
        AddressRange(0x0100, 0x0200, RangeType::BeginInclusive)
    );

    memController.Write(static_cast<uint8_t>(0xFF), 0x0101);
    auto value = memController.Read(0x0101, MemoryAccessType::Byte);
    EXPECT_EQ(0xFF, get<uint8_t>(value));

    memController.UnregisterMemoryResource(rom);

    rom2.get()->Write(static_cast<uint8_t>(0xDD), 0x0001);

    memController.RegisterMemoryResource
    (
        rom2,
        AddressRange(0x0100, 0x0200, RangeType::BeginInclusive)
    );

    value = memController.Read(0x0101, MemoryAccessType::Byte);
    EXPECT_EQ(0xDD, get<uint8_t>(value));
}

TEST(TestMemoryController, LoadMemoryResource) 
{
    MemoryController memController;
    shared_ptr<MemoryInterface> rom(new ROM(0x010));
    memController.RegisterMemoryResource
    (
        rom,
        AddressRange(0x0100, 0x0110, RangeType::BeginInclusive)
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

TEST(TestMemoryController, LoadMemoryResourceAtWrongLocation) 
{
    auto testPassed = false;
    MemoryController memController;
    shared_ptr<MemoryInterface> rom(new ROM(0x010));
    memController.RegisterMemoryResource
    (
        rom,
        AddressRange(0x0100, 0x0110, RangeType::BeginInclusive)
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


TEST(TestMemoryController, LoadMemoryResourceWithOffset) 
{
    MemoryController memController;
    shared_ptr<MemoryInterface> rom(new ROM(0x010));
    memController.RegisterMemoryResource
    (
        rom,
        AddressRange(0x0100, 0x0110, RangeType::BeginInclusive)
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