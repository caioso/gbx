#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

#include "../src/ArithmeticLogicUnitInterface.h"
#include "../src/ClockSource.h"
#include "../src/ControlUnit.h"

using namespace std;
using namespace gbx;

class ControlUnitWrapperForTests : public ControlUnit
{
public:
    ClockSource GetClockSource() { return *_clock;}
    ControlUnitState CtrlUnitState() { return _state; }
};

class ALUMock : public ArithmeticLogicUnitInterface
{
public:
    virtual ~ALUMock() = default;
    MOCK_METHOD(void, RunCycle, ());
};


TEST(TestControlUnit, Construction) 
{
    auto controlUnit = make_shared<ControlUnitWrapperForTests>();

    EXPECT_EQ(ControlUnitState::Fetch, controlUnit->CtrlUnitState());
}

TEST(TestControlUnit, Initialization) 
{
    shared_ptr<ArithmeticLogicUnitInterface> alu = make_shared<ALUMock>();
    auto controlUnit = make_shared<ControlUnitWrapperForTests>();
    controlUnit->Initialize(alu);

    EXPECT_EQ(ControlUnitState::Fetch, controlUnit->CtrlUnitState());

}