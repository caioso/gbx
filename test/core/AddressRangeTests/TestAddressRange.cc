#include <gtest/gtest.h>

#include "AddressRange.h"

using namespace std;
using namespace gbxcore;

TEST(TestAddressRange, RangeAllInclusive) 
{
    AddressRange range(0x0010, 0x0020, RangeType::AllInclusive);

    ASSERT_EQ(0x0010, range.Begin());
    ASSERT_EQ(0x0020, range.End());
}

TEST(TestAddressRange, RangeBeginInclusive) 
{
    AddressRange range(0x0010, 0x0020, RangeType::BeginInclusive);

    ASSERT_EQ(0x0010, range.Begin());
    ASSERT_EQ(0x001F, range.End());
}

TEST(TestAddressRange, RangeEndInclusive) 
{
    AddressRange range(0x0010, 0x0020, RangeType::EndInclusive);

    ASSERT_EQ(0x000F, range.Begin());
    ASSERT_EQ(0x0020, range.End());
}

TEST(TestAddressRange, RangeNoneInclusive) 
{
    AddressRange range(0x0010, 0x0020, RangeType::NoneInclusive);

    ASSERT_EQ(0x000F, range.Begin());
    ASSERT_EQ(0x001F, range.End());
}