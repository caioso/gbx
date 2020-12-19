#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cmath>
#include <thread>

#include <memory>
#include "../src/GBXExceptions.h"
#include "../src/EngineParameters.h"
#include "../src/ClockObserver.h"
#include "../src/ClockSource.h"

using namespace std;
using namespace gbx;

class ClockObserverMock : public ClockObserver
{
public:
    ClockObserverMock() = default;
    virtual ~ClockObserverMock() = default;
    MOCK_METHOD0(OnTick, void(void));
};

TEST(ClockTests, Construction) 
{
    constexpr double GBCPeriod = 1.0/8388608;
    ClockSource clock(EngineParameters::GBCFrequency);

    EXPECT_DOUBLE_EQ(GBCPeriod, clock.Period());
}

TEST(ClockTests, Tick)
{
    ClockSource clock(EngineParameters::GBCFrequency);
    clock.Tick();

    EXPECT_EQ(static_cast<uint32_t>(1), clock.Ticks());

    for (auto i = 0; i < 1000; i++)
    {
        clock.Tick();
    }

    EXPECT_EQ(static_cast<uint32_t>(1001), clock.Ticks());
}

TEST(ClockTests, Observers)
{
    ClockSource clock(EngineParameters::GBCFrequency);
    shared_ptr<ClockObserver> mock = make_shared<ClockObserverMock>();
    auto observer = static_pointer_cast<ClockObserver>(mock);

    clock.Subscribe(observer);

    EXPECT_CALL(*dynamic_pointer_cast<ClockObserverMock>(mock), OnTick());
    clock.Tick();
}

TEST(ClockTests, MultipleObservers)
{
    ClockSource clock(EngineParameters::GBCFrequency);
    shared_ptr<ClockObserver> mock1 = make_shared<ClockObserverMock>();
    auto observer1 = static_pointer_cast<ClockObserver>(mock1);
    shared_ptr<ClockObserver> mock2 = make_shared<ClockObserverMock>();
    auto observer2 = static_pointer_cast<ClockObserver>(mock2);

    clock.Subscribe(observer1);
    clock.Subscribe(observer2);

    EXPECT_CALL(*dynamic_pointer_cast<ClockObserverMock>(mock1), OnTick()).Times(100);
    EXPECT_CALL(*dynamic_pointer_cast<ClockObserverMock>(mock2), OnTick()).Times(100);

    for (auto i = 0; i < 100; i++)
        clock.Tick();
}

TEST(ClockTests, SameObserverRegistered)
{
    auto testPassed = false;
    ClockSource clock(EngineParameters::GBCFrequency);
    shared_ptr<ClockObserver> mock = make_shared<ClockObserverMock>();
    auto observer = static_pointer_cast<ClockObserver>(mock);

    try
    {
        clock.Subscribe(observer);
        clock.Subscribe(observer);
    }
    catch (const ClockSourceException& e)
    {
        testPassed = true;
    }

    EXPECT_TRUE(testPassed);
}