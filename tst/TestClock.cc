#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <chrono>
#include <thread>
#include <memory>

#include "../src/Clock.h"
#include "../src/GBXCoreExceptions.h"
#include "../src/EngineParameters.h"
#include "../src/interfaces/ClockInterface.h"

using namespace std;
using namespace gbx;

TEST(TestClock, Construction) 
{
    constexpr double GBCPeriod = 119;
    Clock clock(EngineParameters::GBCClockPeriod);

    EXPECT_DOUBLE_EQ(GBCPeriod, clock.Period());
}

TEST(TestClock, SingleTickNoDelay)
{
    constexpr uint64_t GBCPeriodInNanoSeconds = 119;
    Clock clock(EngineParameters::GBCClockPeriod);

    auto startTime = chrono::high_resolution_clock::now();
    clock.Tick(1, 0);
    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);

    EXPECT_EQ(static_cast<uint64_t>(1), clock.Ticks());
    EXPECT_TRUE(duration >= chrono::nanoseconds(static_cast<uint64_t>(GBCPeriodInNanoSeconds)));
    EXPECT_TRUE(duration <= chrono::nanoseconds(static_cast<uint64_t>(GBCPeriodInNanoSeconds * 3)));
}

TEST(TestClock, MultipleTickNoDelay)
{
    constexpr uint64_t GBCPeriodInNanoSeconds = 119;
    Clock clock(EngineParameters::GBCClockPeriod);

    auto startTime = chrono::high_resolution_clock::now();
    clock.Tick(4, 0);
    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);

    EXPECT_EQ(static_cast<uint64_t>(4), clock.Ticks());
    EXPECT_TRUE(duration >= chrono::nanoseconds(static_cast<uint64_t>(GBCPeriodInNanoSeconds * 4)));
    EXPECT_TRUE(duration <= chrono::nanoseconds(static_cast<uint64_t>(GBCPeriodInNanoSeconds * 6)));
}

TEST(TestClock, MultipkleTickWithDelay)
{
    constexpr uint64_t GBCPeriodInNanoSeconds = 119;
    Clock clock(EngineParameters::GBCClockPeriod);

    auto startTime = chrono::high_resolution_clock::now();
    clock.Tick(4, GBCPeriodInNanoSeconds * 2);
    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);

    EXPECT_EQ(static_cast<uint64_t>(4), clock.Ticks());
    EXPECT_TRUE(duration >= chrono::nanoseconds(static_cast<uint64_t>(GBCPeriodInNanoSeconds * 2)));
    EXPECT_TRUE(duration <= chrono::nanoseconds(static_cast<uint64_t>(GBCPeriodInNanoSeconds * 4)));
}