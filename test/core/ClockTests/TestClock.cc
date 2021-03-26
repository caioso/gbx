#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <chrono>
#include <thread>
#include <memory>

#include "Clock.h"
#include "GBXCoreExceptions.h"
#include "EngineParameters.h"
#include "ClockInterface.h"

using namespace std;
using namespace gbxcore;

TEST(CoreTests_Clock, Construction) 
{
    constexpr double GBCPeriod = 119;
    Clock clock(EngineParameters::GBCClockPeriod);

    EXPECT_DOUBLE_EQ(GBCPeriod, clock.Period());
}

TEST(CoreTests_Clock, SingleTickNoDelay)
{
    constexpr uint64_t GBCPeriodInNanoSeconds = 119;
    Clock clock(EngineParameters::GBCClockPeriod);

    auto startTime = chrono::high_resolution_clock::now();
    clock.Tick(1, 0);
    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);

    EXPECT_EQ(static_cast<uint64_t>(1), clock.Ticks());
    EXPECT_TRUE(duration >= chrono::nanoseconds(static_cast<uint64_t>(GBCPeriodInNanoSeconds)));
    EXPECT_TRUE(duration <= chrono::nanoseconds(static_cast<uint64_t>(GBCPeriodInNanoSeconds * 4)));
}

TEST(CoreTests_Clock, MultipleTickNoDelay)
{
    constexpr uint64_t GBCPeriodInNanoSeconds = 119;
    Clock clock(EngineParameters::GBCClockPeriod);

    auto startTime = chrono::high_resolution_clock::now();
    clock.Tick(4, 0);
    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);

    EXPECT_EQ(static_cast<uint64_t>(4), clock.Ticks());
    EXPECT_TRUE(duration >= chrono::nanoseconds(static_cast<uint64_t>(GBCPeriodInNanoSeconds * 2)));
    EXPECT_TRUE(duration <= chrono::nanoseconds(static_cast<uint64_t>(GBCPeriodInNanoSeconds * 6)));
}

TEST(CoreTests_Clock, MultipkleTickWithDelay)
{
    constexpr uint64_t GBCPeriodInNanoSeconds = 119;
    Clock clock(EngineParameters::GBCClockPeriod);

    auto startTime = chrono::high_resolution_clock::now();
    clock.Tick(4, GBCPeriodInNanoSeconds * 2);
    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);

    EXPECT_EQ(static_cast<uint64_t>(4), clock.Ticks());
    EXPECT_TRUE(duration >= chrono::nanoseconds(static_cast<uint64_t>(GBCPeriodInNanoSeconds)));
    EXPECT_TRUE(duration <= chrono::nanoseconds(static_cast<uint64_t>(GBCPeriodInNanoSeconds * 4)));
}