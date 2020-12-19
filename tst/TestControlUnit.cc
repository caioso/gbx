#include <gtest/gtest.h>

#include "../src/ControlUnit.h"

using namespace std;
using namespace gbx;

class ControlUnitWrapperForTests : public ControlUnit
{
public:
    ControlUnitState CtrlUnitState() { return _state; }
    FetchSubState CtrlFetchSubstate() { return _fetchSubstate; }
};

TEST(ControlUnitTest, Construction) 
{
    auto controlUnit = make_shared<ControlUnitWrapperForTests>();

    EXPECT_EQ(ControlUnitState::Fetch, controlUnit->CtrlUnitState());
    EXPECT_EQ(FetchSubState::FetchT1, controlUnit->CtrlFetchSubstate());
}

TEST(ControlUnitTest, UpdateFetchState) 
{
    auto controlUnit = make_shared<ControlUnitWrapperForTests>();
    
    controlUnit->Update();

    EXPECT_EQ(ControlUnitState::Fetch, controlUnit->CtrlUnitState());
    EXPECT_EQ(FetchSubState::FetchT2, controlUnit->CtrlFetchSubstate());

    controlUnit->Update();

    EXPECT_EQ(ControlUnitState::Fetch, controlUnit->CtrlUnitState());
    EXPECT_EQ(FetchSubState::FetchT3, controlUnit->CtrlFetchSubstate());

    controlUnit->Update();

    EXPECT_EQ(ControlUnitState::Fetch, controlUnit->CtrlUnitState());
    EXPECT_EQ(FetchSubState::FetchT4, controlUnit->CtrlFetchSubstate());

    controlUnit->Update();

    EXPECT_EQ(ControlUnitState::Fetch, controlUnit->CtrlUnitState());
    EXPECT_EQ(FetchSubState::FetchT1, controlUnit->CtrlFetchSubstate());
}