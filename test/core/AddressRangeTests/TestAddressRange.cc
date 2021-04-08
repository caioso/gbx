#include <gtest/gtest.h>

#include "AddressRange.h"

using namespace std;
using namespace gbxcore;

TEST(CoreTests_AddressRange, RangeAllInclusive) 
{
    AddressRange range(0x0010llu, 0x0020llu, RangeType::AllInclusive);

    ASSERT_EQ(0x0010llu, range.Begin());
    ASSERT_EQ(0x0020llu, range.End());
}

TEST(CoreTests_AddressRange, RangeBeginInclusive) 
{
    AddressRange range(0x0010llu, 0x0020llu, RangeType::BeginInclusive);

    ASSERT_EQ(0x0010llu, range.Begin());
    ASSERT_EQ(0x001Fllu, range.End());
}

TEST(CoreTests_AddressRange, RangeEndInclusive) 
{
    AddressRange range(0x0010llu, 0x0020llu, RangeType::EndInclusive);

    ASSERT_EQ(0x000Fllu, range.Begin());
    ASSERT_EQ(0x0020llu, range.End());
}

TEST(CoreTests_AddressRange, RangeNoneInclusive) 
{
    AddressRange range(0x0010llu, 0x0020llu, RangeType::NoneInclusive);

    ASSERT_EQ(0x000Fllu, range.Begin());
    ASSERT_EQ(0x001Fllu, range.End());
}